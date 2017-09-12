#include "boost/filesystem.hpp"

#include <algorithm>
#include <chrono>
#include <deque>
#include <iostream>
#include <random>
#include <vector>


boost::filesystem::path GenerateFileToBesorted() {
    // Generate the random data
    uint64_t data_size = 128 * 1024 * 1024; // 128 * 2^20 elements ~1GB
    
    boost::filesystem::path data_to_be_sorted_file = boost::filesystem::unique_path();
    std::ofstream data_to_be_sorted(data_to_be_sorted_file.string(), std::ios::binary);
    
    std::random_device random_device;
    std::default_random_engine engine(random_device());
    std::uniform_int_distribution<uint64_t> uniform_dist(0, std::numeric_limits<uint64_t>::max());
    
    for (uint64_t i = 0; i < data_size; ++i) {
        uint64_t random_num = uniform_dist(engine);
        data_to_be_sorted.write(reinterpret_cast<char *>(&random_num), sizeof(uint64_t));
    }
    
    data_to_be_sorted.close();
    
    return data_to_be_sorted_file;
}

std::vector<boost::filesystem::path> GenerateTemporaryFiles(boost::filesystem::path input_file) {
    uint64_t data_size = boost::filesystem::file_size(input_file) / sizeof(uint64_t);
    
    // Split into files
    uint64_t temp_file_size = 16 * 1024 * 1024; // ~128MB
    
    uint64_t num_temp_files = data_size / temp_file_size;
    if (data_size % temp_file_size != 0) {
        ++num_temp_files;
    }
    
    std::ifstream input_file_stream(input_file.string(), std::ios::binary);
    
    std::vector<boost::filesystem::path> temporary_file_paths(num_temp_files);
    for (auto & temp_file_path : temporary_file_paths) {
        temp_file_path = boost::filesystem::unique_path();
        std::ofstream temp_file_stream(temp_file_path.string(), std::ios::binary);
        
        std::vector<uint64_t> buffer(temp_file_size);
        
        input_file_stream.read(reinterpret_cast<char *>(buffer.data()), temp_file_size * sizeof(uint64_t));
        auto actual_bytes_read = input_file_stream.gcount();
        
        std::sort(buffer.begin(), buffer.end());
        
        temp_file_stream.write(reinterpret_cast<char *>(buffer.data()), actual_bytes_read);
    }
    
    return temporary_file_paths;
}

template <class T>
class ifstream_buffer {
public:
    ifstream_buffer(std::ifstream & stream, int max_buffer_size) : stream_(stream), max_buffer_size_(max_buffer_size) {
        refill_buffer();
    }
    
    ifstream_buffer(const ifstream_buffer & other) : stream_(other.stream_), buffer_(other.buffer_), max_buffer_size_(other.max_buffer_size_) {}
    ifstream_buffer & operator=(const ifstream_buffer & other) { stream_ = other.stream_; buffer_ = other.buffer_; max_buffer_size_ = other.max_buffer_size_; return *this; }
    
    ifstream_buffer(ifstream_buffer && other) : stream_(std::move(other.stream_)), buffer_(std::move(other.buffer_)), max_buffer_size_(std::move(other.max_buffer_size_)) {}
    ifstream_buffer & operator=(ifstream_buffer && other) {
        auto & stream_ref = other.stream_.get();
        stream_ = std::ref(stream_ref);
        buffer_ = std::move(other.buffer_);
        max_buffer_size_ = other.max_buffer_size_;
        return *this;
    }
    
    ~ifstream_buffer() = default;
    
    void refill_buffer() {
        // Calculate how much space before we hit max_buffer_size
        
        std::ifstream & stream = stream_.get();
        
        auto buffer_space_remaining = max_buffer_size_ - buffer_.size();
        
        if (buffer_space_remaining > 0) {
            std::vector<T> t_vec(buffer_space_remaining);
            stream.read(reinterpret_cast<char *>(t_vec.data()), buffer_space_remaining * sizeof(T));
            
            auto actual_elements_read = stream.gcount() / sizeof(T);
            
            t_vec.resize(actual_elements_read);
            
            buffer_.insert(buffer_.end(), t_vec.begin(), t_vec.end());
        }
    }
    
    bool empty() const {
        return stream_.get().eof() && buffer_.empty();
    }
    
    T get() {
        T t = buffer_.front();
        buffer_.pop_front();
        
        if (buffer_.empty() && !stream_.get().eof()) {
            refill_buffer();
        }
        
        return t;
    }
    
    T peek() const {
        assert(!buffer_.empty());
        return buffer_.front();
    }
    
private:
    std::reference_wrapper<std::ifstream> stream_;
    std::deque<T> buffer_;
    int max_buffer_size_;
};

std::vector<boost::filesystem::path> MergeFiles(const std::vector<boost::filesystem::path> & temp_files, int n_way) {
    std::vector<boost::filesystem::path> output_temp_files;
    
    for (int i = 0; i < temp_files.size(); i+= n_way) {
        std::vector<std::ifstream> temp_ifstreams;
        temp_ifstreams.reserve(n_way);
        
        for (int j = 0; j < n_way; ++j) {
            if (i + j < temp_files.size()) {
                temp_ifstreams.emplace_back(temp_files[i+j].string(), std::ios::binary);
            }
        }
        
        // For each n-way merge, make an output temp file
        boost::filesystem::path output_temp_file_path = boost::filesystem::unique_path();
        output_temp_files.push_back(output_temp_file_path);
        std::ofstream output_temp_filestream(output_temp_file_path.string(), std::ios::binary);
        
        // Make a buffer for each temp_ifstream
        std::vector<ifstream_buffer<uint64_t>> temp_ifstream_buffers;
        for (auto & temp_ifstream : temp_ifstreams) {
            temp_ifstream_buffers.emplace_back(temp_ifstream, 1024 * 1024); // 8 MB buffer
        }
        
        while (true) {
            // Go through and remove the empty buffers
            for (auto it = temp_ifstream_buffers.begin(); it != temp_ifstream_buffers.end();) {
                if (it->empty()) {
                    it = temp_ifstream_buffers.erase(it);
                } else {
                    ++it;
                }
            }
            
            if (temp_ifstream_buffers.empty()) {
                break;
            }
            
            // Find minimum buffer
            auto min_buffer = std::min_element(temp_ifstream_buffers.begin(), temp_ifstream_buffers.end(), [](const auto & buffer1, const auto & buffer2) { return buffer1.peek() < buffer2.peek(); });
            
            // Write the element
            uint64_t min_elem = min_buffer->get();
            output_temp_filestream.write(reinterpret_cast<char *>(&min_elem), sizeof(uint64_t));
        }
    }
    
    return output_temp_files;
}

int main() {
    auto start_random_gen = std::chrono::steady_clock::now();
    auto data_to_be_sorted_file = GenerateFileToBesorted();
    auto stop_random_gen = std::chrono::steady_clock::now();
    
    std::cout << "Random num gen took: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop_random_gen - start_random_gen).count() << " ms" << std::endl;
    
    std::cout << "Generated random numbers into data file: " << boost::filesystem::canonical(data_to_be_sorted_file.string()) << std::endl;
    
    // Split into chunks
    auto start_split = std::chrono::steady_clock::now();
    auto temp_files = GenerateTemporaryFiles(data_to_be_sorted_file);
    auto stop_split = std::chrono::steady_clock::now();
    
    std::cout << "Splitting took: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop_split - start_split).count() << " ms" << std::endl;
    
    // Now merge
    int n_way_merge = 16; // pick the n
    
    auto start_merge = std::chrono::steady_clock::now();
    while (temp_files.size() != 1) {
        temp_files = MergeFiles(temp_files, n_way_merge);
    }
    auto stop_merge = std::chrono::steady_clock::now();
    
    std::cout << std::endl;
    std::cout << temp_files[0].string() << std::endl;

    std::cout << "Merging took: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop_merge - start_merge).count() << " ms" << std::endl;
}
