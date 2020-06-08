#ifndef __MOVING_ARRAY__
#define __MOVING_ARRAY__

template <typename T, size_t Size> 
class MovingArray {
    protected:
        T values[Size];
        size_t idx = 0;
    public: 
        MovingArray& push(T in) {
            values[idx++] = in;
            if (idx == Size) {
                idx = 0;
            }
            return *this;
        }
    
        T getMax () {
            T maxVal = values[0];
            for (size_t i = 1; i < Size; i++) {
                if (maxVal < values[i]) {
                    maxVal = values[i];
                }
            }
            return maxVal;
        }
        static_assert(Size > 0, "Size of array shall be > 0");
};

#endif