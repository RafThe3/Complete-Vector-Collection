# Perfect-Collection

This project attempts to create a "perfect" collection system, which is defined to be a collection system with all the readily available functions and implementations to use in .cpp programs. The collection includes STL libraries, standard ADT behaviors, such as list, stack, and queue behaviors, and basic exception handling. To test the collection, first create a .cpp executable, then test every function in the header file. It is recommended print the vector after each function call, and it is optional to document the compute time of the sorting and searching functions to demonstrate time complexity of different algorithms.

Note (1): quickSort() uses last element partition, otherwise known as the Lomuto partition scheme. This produces $O(n^2)$ performance and potentially $O(n)$ recursion depth in worst cases. This design choice was made for simplicity of the algorithm, while other partition schemes are potentially more performant, but complex to implement. sort() only uses quickSort() when the collection size is within the interval $50 < n <= 150000$.

Note (2): This collection forbids duplicate values into the vector. If the user attempts to enter a duplicate value into the vector using either insertion function, the function will return false, without adding the value into the vector.
