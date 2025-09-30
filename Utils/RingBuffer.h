#pragma once
#include <array>
#include<algorithm>
#include <stdexcept>

//リングバッファの独自配列
template <typename T, std::size_t N>
class RingBuffer {
public:
    RingBuffer() {}
    ~RingBuffer() {}

    void push(const T& val) {
        buffer[head] = val;
        head = (head + 1) % N;

        if (full) {
            tail = (tail + 1) % N;  // 上書きなので tail も進める
        }

        full = (head == tail);
    }

    void pushFront(const T& val) {
        tail = (tail == 0) ? N - 1 : tail - 1;  // tail を逆方向に移動
        buffer[tail] = val;

        if (full) {
            head = (head == 0) ? N - 1 : head - 1;  // 上書きになるので head も巻き戻す
        }

        full = (head == tail);
    }

    void pushFront(T&& val) {
        tail = (tail == 0) ? N - 1 : tail - 1;
        buffer[tail] = std::move(val);

        if (full) {
            head = (head == 0) ? N - 1 : head - 1;
        }

        full = (head == tail);
    }

    void pop() {
        if (isEmpty()) return;

        tail = (tail + 1) % N;
        full = false;
    }

    T& at(std::size_t index) {
        if (isEmpty()) {
            throw std::runtime_error("RingBuffer::at() - buffer is empty");
        }

        if (index >= size()) {
            throw std::out_of_range("Index out of range in RingBuffer::at()");
        }
        std::size_t realIndex = (tail + index) % N;
        return buffer[realIndex];
    }

    const T& at(std::size_t index) const {
        if (isEmpty()) {
            throw std::runtime_error("RingBuffer::at() - buffer is empty");
        }
        if (index >= size()) {
            throw std::out_of_range("Index out of range in RingBuffer::at()");
        }
        std::size_t realIndex = (tail + index) % N;
        return buffer[realIndex];
    }

    T& front() {
        if (isEmpty()) {
            throw std::runtime_error("Buffer is empty");
        }
        return buffer[tail];
    }

    const T& front() const {
        if (isEmpty()) {
            throw std::runtime_error("Buffer is empty");
        }
        return buffer[tail];
    }

    T frontAndPop() {
        /*if (isEmpty()) {
            throw std::runtime_error("Buffer is empty");
        }*/
        T val = std::move(buffer[tail]);
        tail = (tail + 1) % N;
        full = false;
        return val;
    }

    T& back() {
        if (isEmpty()) {
            throw std::runtime_error("Buffer is empty");
        }
        std::size_t idx = (head == 0) ? N - 1 : head - 1;
        return buffer[idx];
    }

    const T& back() const {
        if (isEmpty()) {
            throw std::runtime_error("Buffer is empty");
        }
        std::size_t idx = (head == 0) ? N - 1 : head - 1;
        return buffer[idx];
    }

    T backAndPop() {
        /*if (isEmpty()) {
            throw std::runtime_error("Buffer; is empty");
        }*/
        head = (head == 0) ? N - 1 : head - 1;
        full = false;
        return std::move(buffer[head]);
    }

    bool isFull() const {
        return full;
    }

    //空の時はTrue
    bool isEmpty() const {
        return (!full && head == tail);
    }

    std::size_t size() const {
        if (full) return N;
        if (head >= tail) return head - tail;
        return N + head - tail;
    }

    constexpr std::size_t capacity() const {
        return N;
    }

    std::size_t available() const {
        return N - size();
    }
    void clear() {
        head = tail = 0;
        full = false;
    }

private:
    std::array<T, N> buffer{};
    std::size_t head = 0;
    std::size_t tail = 0;
    bool full = false;
};