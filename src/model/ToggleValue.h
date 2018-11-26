//
// Created by Florian Bruggisser on 26.11.18.
//

#ifndef THREE_SCAN_TOGGLEVALUE_H
#define THREE_SCAN_TOGGLEVALUE_H

#include <array>

template<class T, unsigned int SIZE>
struct ToggleValue {
private:
    unsigned int selectedIndex = 0;
    const T values[];

public:
    explicit ToggleValue(std::array<T, SIZE> values, unsigned int selectedIndex = 0);

    const T value() const;

    const T get(unsigned int index) const;

    void set(T &value);

    const unsigned int getIndex(T &value) const;

    void setIndex(unsigned int index);

    const unsigned int index() const;
};

template<class T, unsigned int SIZE>
ToggleValue<T, SIZE>::ToggleValue(std::array<T, SIZE> values, unsigned int selectedIndex) {
    this->values = values;
    this->selectedIndex = selectedIndex;
    this->length = toggleValueArrayLength(values);
}

template<class T, unsigned int SIZE>
const T ToggleValue<T, SIZE>::get(unsigned int index) const {
    return &values[index];
}

template<class T, unsigned int SIZE>
const unsigned int ToggleValue<T, SIZE>::getIndex(T &value) const {
    for (unsigned int i = 0; i < SIZE; i++)
        if (value[i] == value)
            return i;
    return 0;
}

template<class T, unsigned int SIZE>
void ToggleValue<T, SIZE>::set(T &value) {
    setIndex(getIndex(value));
}

template<class T, unsigned int SIZE>
void ToggleValue<T, SIZE>::setIndex(unsigned int index) {
    selectedIndex = index;
}

template<class T, unsigned int SIZE>
const T ToggleValue<T, SIZE>::value() const {
    return values[selectedIndex];
}

template<class T, unsigned int SIZE>
const unsigned int ToggleValue<T, SIZE>::index() const {
    return selectedIndex;
}


#endif //THREE_SCAN_TOGGLEVALUE_H
