/**
 * @file deque.cpp
 * Implementation of the Deque class.
 *
 */

/* Constructor for the Deque class */
template <class T>
Deque<T>::Deque(){
    // NOTE DEQUE IS NOT CIRCULAR (DO NOT WRAP USING MODULUS)
    // k1 is the size of the empty space on the left side
    k1 = 0;
    // k2 is the size of the data
    k2 = 0;
}

/**
 * Adds the parameter object to the right of the Deque.
 *
 * @param newItem object to be added to the Deque.
 */
template <class T>
void Deque<T>::pushR(T const& newItem)
{
    // There's room on the right side, so don't modify the vector's size
    if((int)data.size() > k1 + k2) {
        data[k1+k2] = newItem;
        k2++;
    }
    // There's no room on the right side
    else {
        data.push_back(newItem);
        k2++;
    }
}

/**
 * Removes the object at the left of the Deque, and returns it to the
 * caller.
 *
 * See .h file for implementation notes. 
 *
 * @return The leftmost item of the Deque.
 */
template <class T>
T Deque<T>::popL()
{
    T ret_val = data[k1];
    // Mark the next index on the left as invalid
    k1++;
    k2--;
    // Size of data is less than half of actual vector's size
    if(2*k2 <= (int)data.size()) {
        vector<T> newdata;
        for(int i = 0; i < k2; i++) {
            newdata.push_back(data[k1+i]);
        }
        data = newdata;
        // No more empty space in front
        k1 = 0;
    }
    return ret_val;
}
/**
 * Removes the object at the right of the Deque, and returns it to the
 * caller.
 *
 * @return The rightmost item of the Deque.
 */
template <class T>
T Deque<T>::popR()
{
    T ret_val = data[k1+k2-1];
    // Mark the next index on the right as invalid
    k2--;
    // Size of data is less than half of actual vector's size
    if(2*k2 <= (int)data.size()) {
        vector<T> newdata;
        for(int i = 0; i < k2; i++) {
            newdata.push_back(data[k1+i]);
        }
        data = newdata;
        // No more empty space in front
        k1 = 0;
    }
    return ret_val;
}

/**
 * Finds the object at the left of the Deque, and returns it to the
 * caller. Unlike popL(), this operation does not alter the deque.
 *
 * @return The item at the left of the deque.
 */
template <class T>
T Deque<T>::peekL()
{
    return data[k1];
}

/**
 * Finds the object at the right of the Deque, and returns it to the
 * caller. Unlike popR(), this operation does not alter the deque.
 *
 * @return the value of The item at the right of the deque.
 */
template <class T>
T Deque<T>::peekR()
{
    return data[k1+k2-1];
}

/**
 * Determines if the Deque is empty.
 *
 * @return bool which is true if the Deque is empty, false otherwise.
 */
template <class T>
bool Deque<T>::isEmpty() const
{
    return (k2 == 0);
}
