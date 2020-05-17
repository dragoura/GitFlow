#ifndef BUFFERED_CHANNEL_H_
#define BUFFERED_CHANNEL_H_

#include <queue>
#include <mutex>
#include <condition_variable>

template<class T>
class BufferedChannel {
public:
	explicit BufferedChannel(int size): buffer_size_(size), is_closed(false) {}

	void Send(T value) {
		std::unique_lock<std::mutex> locker(mutex_);
		full_queue_.wait(locker, [this]() {
			if (is_closed) {
				throw std::runtime_error("Channel is closed!");
			}
			return queue_.size() < buffer_size_;
		});
		queue_.push(value);
		full_queue_.notify_all();
	}

	std::pair<T, bool> Recv() {
		std::unique_lock<std::mutex> locker(mutex_);
		empty_queue_.wait(locker, [this]() {
			return is_closed || !queue.empty();
		});

		if (queue_.empty()) {
			empty_queue_.notify_all();
			return std::make_pair(T(), false);
		}
		T value = queue_.front();
		queue_.pop();
		empty_queue_.notify_one();
		return std::make_pair(value, true);
	}

	void Close() {
		is_closed = true;
		std::unique_lock<std::mutex> lock(mutex);
		full_queue_.notify_all();
		empty_queue_.notify_all();
	}

private:
	int buffer_size_;
	std::queue<T> queue_;
	std::mutex mutex_;
	bool is_closed;
	std::condition_variable full_queue_;
	std::condition_variable empty_queue_;
};

#endif // BUFFERED_CHANNEL_H_