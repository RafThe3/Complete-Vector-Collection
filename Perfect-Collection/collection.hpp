#pragma once

#include <vector>
#include <cstddef>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <utility>
#include <stdexcept>
#include <iomanip>
#include <type_traits>
#include <limits>

template <typename T>
class Collection
{
    private:
    std::vector<T> items;
    bool isSorted = true;

    // Searching
    std::size_t linearSearch(const T& item) const;
    std::size_t binarySearch(const T& item) const;

    // Sorting
    void insertionSort(std::vector<T> &v);
    void quickSort(std::vector<T> &v, std::size_t low, std::size_t high);
    void mergeSort(std::vector<T> &v);

    public:

    // Constructor
    Collection() = default;

    // Insertion
    bool push_front(const T& item);
    bool push_back(const T& item);
    bool insert(const T& item, std::size_t index);
    bool insertSorted(const T& item);

    // Deletion
    bool remove(const T& item);
    bool removeAt(std::size_t index);
    bool pop_front(T& out);
    bool pop_back(T& out);
    bool pop_front();
    bool pop_back();
    bool clear();

    // Replacement
    bool replace(const T& oldVal, const T& newVal);
    bool replaceAt(const T& item, std::size_t index);
    bool swap(const T& oldVal, const T& newVal);
    bool swapAt(const T& item, std::size_t index);

    // Searching
    bool contains(const T& item) const;
    const T& at(std::size_t index) const;
    std::size_t search(const T& item) const;

    // Sorting
    bool sort();
    
    // Global
    const T& front() const;
    const T& back() const;
    bool isEmpty() const;
    std::size_t getSize() const;
    std::string toString() const;

    // Saving and Loading
    bool save(const std::string& filename) const;
    bool load(const std::string& filename);
};

#pragma region Insertion

// ------------------------Insertion--------------------------

// Inserts a unique value at the beginning of the collection.
// The sorted-state flag is updated by comparing the new first value with the old first value.
template <typename T>
inline bool Collection<T>::push_front(const T& item)
{
    if (!contains(item))
    {
        // An empty collection is sorted; otherwise check whether the new front preserves order.
        if (isEmpty())
        {
            isSorted = true;
        }
        else if (front() < item)
        {
            isSorted = false;
        }

        items.insert(items.begin(), item);
        return true;
    }
    
    return false;
}

// Appends a unique value to the collection.
// Appending a value smaller than the previous last value breaks ascending order.
template <typename T>
inline bool Collection<T>::push_back(const T& item)
{
    if (!contains(item))
    {
        // An empty collection is sorted; otherwise check whether the new back preserves order.
        if (isEmpty())
        {
            isSorted = true;
        }
        else if (item < back())
        {
            isSorted = false;
        }

        items.push_back(item);
        return true;
    }

    return false;
}

// Inserts a unique value at the requested position.
// An arbitrary position can disrupt ordering, so the collection is marked unsorted.
template <typename T>
inline bool Collection<T>::insert(const T& item, std::size_t index)
{
    if (!contains(item) && index <= getSize())
    {
        items.insert(items.begin() + static_cast<typename std::vector<T>::difference_type>(index), item);
        isSorted = false;
        return true;
    }
    
    return false;
}

// Inserts a unique value into its correct position in an already sorted collection.
// lower_bound finds the first position whose value is not less than the new value.
template <typename T>
inline bool Collection<T>::insertSorted(const T& item)
{
    if (!isSorted)
    {
        return false;
    }

    auto position = std::lower_bound(items.begin(), items.end(), item);

    if (position != items.end() && *position == item)
    {
        return false;
    }

    items.insert(position, item);
    return true;
}

// -----------------------------------------------------------

#pragma endregion

#pragma region Deletion

// -------------------------Deletion--------------------------

// Finds and removes a value. getSize() is the sentinel returned when search misses.
template <typename T>
inline bool Collection<T>::remove(const T& item)
{
    // checks if the value already exists. if so, remove it

    std::size_t index = search(item);

    if (index != getSize())
    {
        items.erase(items.begin() + static_cast<typename std::vector<T>::difference_type>(index));
        if (getSize() <= 1)
        {
            isSorted = true;
        }
        return true;
    }

    return false;
}

// Removes the value at index when the index is within the collection bounds.
template <typename T>
inline bool Collection<T>::removeAt(std::size_t index)
{
    if (index < getSize())
    {
        items.erase(items.begin() + static_cast<typename std::vector<T>::difference_type>(index));
        if (getSize() <= 1)
        {
            isSorted = true;
        }
        return true;
    }

    return false;
}

// Copies the first value to out, then removes it from the collection.
template <typename T>
inline bool Collection<T>::pop_front(T& out)
{
    if (!isEmpty())
    {
        out = front();
        items.erase(items.begin());
        if (getSize() <= 1)
        {
            isSorted = true;
        }
        return true;
    }

    return false;
}

// Copies the last value to out, then removes it from the collection.
template <typename T>
inline bool Collection<T>::pop_back(T& out)
{
    if (!isEmpty())
    {
        out = back();
        items.pop_back();
        if (getSize() <= 1)
        {
            isSorted = true;
        }
        return true;
    }

    return false;
}

// Removes the first value out of the collection.
template <typename T>
inline bool Collection<T>::pop_front()
{
    if (!isEmpty())
    {
        items.erase(items.begin());
        if (getSize() <= 1)
        {
            isSorted = true;
        }
        return true;
    }
    
    return false;
}

// Removes the last value out of the collection.
template <typename T>
inline bool Collection<T>::pop_back()
{
    if (!isEmpty())
    {
        items.pop_back();
        if (getSize() <= 1)
        {
            isSorted = true;
        }
        return true;
    }
    
    return false;
}

// Removes every value, restores the empty collection sorted state, and reports whether it changed.
template <typename T>
inline bool Collection<T>::clear()
{
    std::size_t originalSize = getSize();
    items.clear();
    isSorted = true;
    return getSize() < originalSize;
}

// -----------------------------------------------------------

#pragma endregion

#pragma region Replacement

// ----------------------Replacement--------------------------

// Replaces oldVal with newVal, provided oldVal exists and newVal is not already present.
// Neighbor comparisons determine whether a previously sorted collection remains sorted.
template <typename T>
inline bool Collection<T>::replace(const T& newVal, const T& oldVal)
{
    std::size_t newIndex = search(newVal);
    std::size_t oldIndex = search(oldVal);

    if (newIndex == getSize() && oldIndex != getSize())
    {
        if (isSorted)
        {
            // The new value must fit between its neighbors to preserve ascending order.
            bool orderedBefore = oldIndex == 0 || items[oldIndex - 1] < newVal;
            bool orderedAfter = oldIndex + 1 == getSize() || newVal < items[oldIndex + 1];

            isSorted = orderedBefore && orderedAfter;
        }

        items[oldIndex] = newVal;
        return true;
    }

    return false;
}

// Replaces the value at index, provided the index is valid and item is not already present.
// Neighbor comparisons preserve the sorted-state flag when the replacement fits in order.
template <typename T>
inline bool Collection<T>::replaceAt(const T& item, std::size_t index)
{
    if (index < getSize() && search(item) == getSize())
    {
        if (isSorted)
        {
            // The new value must fit between its neighbors to preserve ascending order.
            bool orderedBefore = index == 0 || items[index - 1] < item;
            bool orderedAfter = index + 1 == getSize() || item < items[index + 1];

            isSorted = orderedBefore && orderedAfter;
        }

        items[index] = item;
        return true;
    }

    return false;
}

// Exchanges the positions of two existing values. A real exchange may disrupt sorted order.
template <typename T>
inline bool Collection<T>::swap(const T& newVal, const T& oldVal)
{
    std::size_t newIndex = search(newVal);
    std::size_t oldIndex = search(oldVal);

    if (newIndex != getSize() && oldIndex != getSize())
    {
        if (newIndex != oldIndex)
        {
            std::swap(items[newIndex], items[oldIndex]);
            isSorted = false;
        }

        return true;
    }

    return false;
}

// Exchanges the position of item with the value at index, after validating both positions.
template <typename T>
inline bool Collection<T>::swapAt(const T& item, std::size_t index)
{
    std::size_t found = search(item);

    if (found != getSize() && index < getSize())
    {
        if (found != index)
        {
            std::swap(items[found], items[index]);
            isSorted = false;
        }

        return true;
    }

    return false;
}

// -----------------------------------------------------------

#pragma endregion

#pragma region Searching

// ------------------------Searching--------------------------

// Reports whether item is present, using the collection current search strategy.
template <typename T>
inline bool Collection<T>::contains(const T& item) const
{
    std::size_t found = search(item);
    return found != getSize();
}

// Returns a read-only reference to the value at index; throws when index is out of range.
template <typename T>
inline const T& Collection<T>::at(std::size_t index) const
{
    if (index < getSize())
    {
        return items[index];
    }
    
    throw std::out_of_range("Index out of range");
}

// Selects binary search for sorted data and linear search for unsorted data.
// Both search algorithms return getSize() when the value is not present.
template <typename T>
inline std::size_t Collection<T>::search(const T& item) const
{
    return isSorted ? binarySearch(item) : linearSearch(item);
}

#pragma region Search Algorithms

// Checks each element in order and returns the first matching index.
template <typename T>
inline std::size_t Collection<T>::linearSearch(const T& item) const
{
    for (std::size_t i = 0; i < getSize(); i++)
    {
        if (items[i] == item)
        {
            return i;
        }
    }

    return getSize();
}

// Uses a lower-bound search over the half-open range [low, high).
// Equality then confirms whether the value at the insertion position is a match.
template <typename T>
inline std::size_t Collection<T>::binarySearch(const T& item) const
{
    // high is exclusive, so the active binary-search interval is [low, high).
    std::size_t low = 0;
    std::size_t high = getSize();

    while (low < high)
    {
        std::size_t middle = low + (high - low) / 2;

        if (items[middle] < item)
        {
            low = middle + 1;
        }
        else
        {
            high = middle;
        }
    }

    if (low < getSize() && items[low] == item)
    {
        return low;
    }

    return getSize();
}

#pragma endregion

// -----------------------------------------------------------

#pragma endregion

#pragma region Sorting

// ------------------------Sorting----------------------------

// Sorts only when the sorted-state flag indicates that sorting is needed.
// The algorithm is selected by collection size; sorting restores the sorted-state flag.
template <typename T>
inline bool Collection<T>::sort()
{
    if (isSorted)
    {
        return false;
    }

    std::size_t size = getSize();

    if (size <= 1)
    {
        isSorted = true;
        return false;
    }

    if (size <= 50)
    {
        insertionSort(items);
    }
    else if (50 < size && size <= 150000)
    {
        quickSort(items, 0, size - 1);
    }
    else if (size > 150000)
    {
        mergeSort(items);
    }

    isSorted = true;
    return true;
}

#pragma region Sorting Algorithms

// Grows a sorted prefix by inserting each next value into its correct position.
template <typename T>
inline void Collection<T>::insertionSort(std::vector<T> &v)
{
    for (std::size_t i = 1; i < v.size(); i++)
    {
        T temp = v[i];
        std::size_t j = i;
        while (j > 0 && temp < v[j - 1])
        {
            v[j] = v[j - 1];
            j--;
        }
        v[j] = temp;
    }
}

// Applies quicksort to the inclusive range [low, high], using the last value as pivot.
template <typename T>
inline void Collection<T>::quickSort(std::vector<T> &v, std::size_t low, std::size_t high)
{
    // Base case: if there is one or zero elements,
    // the portion of the array is already sorted.
    if (low >= high)
    {
        return;
    }

    // Choose the last element as the pivot.
    T pivot = v[high];

    // i keeps track of the position where the next
    // element smaller than the pivot should be placed.
    std::size_t i = low;

    // Check each element before the pivot.
    for (std::size_t j = low; j < high; j++)
    {
        // If the current element is smaller than the pivot,
        // move it into the section containing smaller elements.
        if (v[j] < pivot)
        {
            std::swap(v[i], v[j]);
            i++;
        }
    }

    // Place the pivot between the smaller and larger elements.
    std::swap(v[i], v[high]);

    if (i > low)
    {
         // Recursively sort the elements to the left of the pivot.
        quickSort(v, low, i - 1);
    }

    // Recursively sort the elements to the right of the pivot.
    quickSort(v, i + 1, high);
}

// Recursively divides the vector, sorts each half, and merges the halves in order.
template <typename T>
inline void Collection<T>::mergeSort(std::vector<T> &v)
{
    // Base case:
    // An array with 0 or 1 elements is already sorted.
    if (v.size() <= 1)
    {
        return;
    }

    // Temporary vectors used to store the two halves.
    std::vector<T> left, right;

    // Copy the first half of the array into left.
    for (std::size_t i = 0; i < v.size() / 2; i++)
    {
        left.push_back(v[i]);
    }

    // Copy the second half of the array into right.
    for (std::size_t i = v.size() / 2; i < v.size(); i++)
    {
        right.push_back(v[i]);
    }

    // Recursively sort both halves.
    mergeSort(left);
    mergeSort(right);

    // Merge the two sorted halves back into the original array.

    // Index for the left vector.
    std::size_t leftIndex = 0;

    // Index for the right vector.
    std::size_t rightIndex = 0;

    // Index for the original array.
    std::size_t arrayIndex = 0;

    // Continue while both halves still contain elements.
    while (leftIndex < left.size() && rightIndex < right.size())
    {
        // Take the smaller element from the two halves.
        // <= handles equal elements by taking from the left.
        if (!(right[rightIndex] < left[leftIndex]))
        {
            v[arrayIndex] = left[leftIndex];
            leftIndex++;
        }
        else
        {
            v[arrayIndex] = right[rightIndex];
            rightIndex++;
        }

        // Move to the next position in the original array.
        arrayIndex++;
    }

    // If elements remain in left, copy them into the array.
    while (leftIndex < left.size())
    {
        v[arrayIndex] = left[leftIndex];
        arrayIndex++;
        leftIndex++;
    }

    // If elements remain in right, copy them into the array.
    while (rightIndex < right.size())
    {
        v[arrayIndex] = right[rightIndex];
        arrayIndex++;
        rightIndex++;
    }

    // Free the vectors to preserve space in memory
    free(left);
    free(right);
}

#pragma endregion

// -----------------------------------------------------------

#pragma endregion

#pragma region Saving and Loading

// -------------------Saving and Loading----------------------

// Writes one value per line. Strings are quoted to preserve whitespace and special characters.
// Floating-point output uses enough significant digits for a precision-preserving round trip.
template <typename T>
inline bool Collection<T>::save(const std::string& filename) const
{
    std::ofstream file(filename);

    if (!file)
    {
        //throw std::runtime_error("Unable to open file");
        return false;
    }

    // max_digits10 preserves enough significant digits to reload floating-point values accurately.
    if constexpr (std::is_floating_point_v<T>)
    {
        file << std::setprecision(std::numeric_limits<T>::max_digits10);
    }

    for (const auto& item : items)
    {
        if constexpr (std::is_same_v<T, std::string>)
        {
            file << std::quoted(item) << '\n';
        }
        else
        {
            file << item << '\n';
        }
    }

    file.close();

    if (!file)
    {
        //throw std::runtime_error("Error writing file");
        return false;
    }

    return true;
}

// Reads into a temporary vector so current contents are not replaced until reading succeeds.
// Duplicate values are ignored, and sorted state is recalculated after loading.
template <typename T>
inline bool Collection<T>::load(const std::string& filename)
{
    std::ifstream file(filename);

    if (!file)
    {
        //throw std::runtime_error("Unable to open file");
        return false;
    }

    // Build replacement contents separately; items is updated only after reading succeeds.
    std::vector<T> loaded;
    T item;

    while (true)
    {
        // Stop if the previous extraction already reached EOF.
        if (file.eof())
        {
            break;
        }

        file >> std::ws;

        if (file.bad())
        {
            //throw std::runtime_error("Error reading file");
            return false;
        }

        if (file.eof())
        {
            break;
        }

        if (!file)
        {
            //throw std::runtime_error("Error reading file");
            return false;
        }

        if constexpr (std::is_same_v<T, std::string>)
        {
            if (!(file >> std::quoted(item)))
            {
                //throw std::runtime_error("Error reading file");
                return false;
            }
        }
        else
        {
            if (!(file >> item))
            {
                //throw std::runtime_error("Error reading file");
                return false;
            }
        }

        if (std::find(loaded.begin(), loaded.end(), item) == loaded.end())
        {
            loaded.push_back(item);
        }
    }

    file.close();

    if (!file)
    {
        //throw std::runtime_error("Error reading file");
        return false;
    }

    // Commit the loaded data, then synchronize the sorted-state flag.
    items.swap(loaded);
    isSorted = std::is_sorted(items.begin(), items.end());
    return true;
}

// -----------------------------------------------------------

#pragma endregion

#pragma region Global

// ------------------------Global-----------------------------

// Returns a read-only reference to the first value; throws if the collection is empty.
template <typename T>
inline const T& Collection<T>::front() const
{
    if (!isEmpty())
    {
        return items[0];
    }
    else
    {
        throw std::out_of_range("Empty vector");
    }
}

// Returns a read-only reference to the last value; throws if the collection is empty.
template <typename T>
inline const T& Collection<T>::back() const
{
    if (!isEmpty())
    {
        return items[getSize() - 1];
    }
    else
    {
        throw std::out_of_range("Empty vector");
    }
}

// Returns true when the underlying vector contains no elements.
template <typename T>
inline bool Collection<T>::isEmpty() const
{
    return items.empty();
}

// Returns the number of values currently stored in the collection.
template <typename T>
inline std::size_t Collection<T>::getSize() const
{
    return items.size();
}

// Builds a space-separated string representation of the collection.
template <typename T>
inline std::string Collection<T>::toString() const
{
    std::stringstream ss;

    for (const auto& item : items)
    {
        ss << item << " ";
    }

    return ss.str();
}

// -----------------------------------------------------------

#pragma endregion