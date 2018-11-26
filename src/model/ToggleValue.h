//
// Created by Florian Bruggisser on 26.11.18.
//

#ifndef THREE_SCAN_TOGGLEVALUE_H
#define THREE_SCAN_TOGGLEVALUE_H

#include <array>

#define toggleValueArrayLength(x) (sizeof(x) / sizeof(*x))

template<class T>
struct ToggleValue {
private:
    const unsigned int length = 0;
    unsigned int selectedIndex = 0;
    const T values[];

public:
    ToggleValue(const T values[], unsigned int selectedIndex = 0);

    const T value() const;

    const T get(unsigned int index) const;

    void set(T &value);

    const unsigned int getIndex(T &value) const;

    void setIndex(unsigned int index);

    const unsigned int index() const;
};

template<class T>
ToggleValue<T>::ToggleValue(const T values[], unsigned int selectedIndex
) {
    this->values = values;
    this->selectedIndex = selectedIndex;
    this->length = toggleValueArrayLength(values);
}

template<class T>
const T ToggleValue<T>::get(unsigned int index) const {
    return &values[index];
}

template<class T>
const unsigned int ToggleValue<T>::getIndex(T &value) const {
    for (unsigned int i = 0; i < length; i++)
        if (value[i] == value)
            return i;
    return 0;
}

template<class T>
void ToggleValue<T>::set(T &value) {
    setIndex(getIndex(value));
}

template<class T>
void ToggleValue<T>::setIndex(unsigned int index) {
    selectedIndex = index;
}

template<class T>
const T ToggleValue<T>::value() const {
    return values[selectedIndex];
}

template<class T>
const unsigned int ToggleValue<T>::index() const {
    return selectedIndex;
}


#endif //THREE_SCAN_TOGGLEVALUE_H
