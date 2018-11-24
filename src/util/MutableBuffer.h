//
// Created by Florian Bruggisser on 24.11.18.
//

#ifndef THREE_SCAN_MUTABLEBUFFER_H
#define THREE_SCAN_MUTABLEBUFFER_H

template<class T>
class MutableBuffer {
private:
    typedef T *TPtr;

    TPtr *data;

    const unsigned int maxSize;

    unsigned int index;

public:
    explicit MutableBuffer(unsigned int maxSize);

    ~MutableBuffer();

    void add(T value);

    void reset();

    void clear();

    const unsigned int length() const {
        return index;
    }

    const unsigned int getMaxSize() const {
        return maxSize;
    }

    TPtr operator[](int i) const { return data[i]; }

    TPtr &operator[](int i) { return data[i]; }
};

template<class T>
MutableBuffer<T>::MutableBuffer(unsigned int maxSize) {
    this->maxSize = maxSize;
    this->data = new TPtr[maxSize];
    reset();
}

template<class T>
MutableBuffer<T>::~MutableBuffer() {
    clear();
    delete data;
}

template<class T>
void MutableBuffer<T>::add(T value) {
    data[++index] = value;
}

template<class T>
void MutableBuffer<T>::reset() {
    index = 0;
}

template<class T>
void MutableBuffer<T>::clear() {
    for (auto i = 0; i < length(); i++) {
        delete data[i];
    }
    reset();
}


#endif //THREE_SCAN_MUTABLEBUFFER_H
