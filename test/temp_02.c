#include <stdio.h>



int binary_search(int arr[], int l, int r, int trg) {
    if(l > r) {
        return -1;
    }
    int mid = (l + r) / 2;
    if(arr[mid] > trg) {
        return binary_search(arr, l, mid - 1, trg);
    }else if(arr[mid] < trg) {
        return binary_search(arr, mid, r, trg);
    }else if(arr[mid] == trg) {
        return mid;
    }
}



int main() {
    int n = 10;
    int arr[] = {-8, 1, 3, 4, 6, 8, 13, 15, 19, 22};
    printf("trg idx: %d\n", binary_search(arr, 0, n - 1, 3));
    return 0;
}