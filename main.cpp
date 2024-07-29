#include <SDL2/SDL.h>
#include <iostream>
#include <ctime>
#include <cstring> // For memcpy

using namespace std;

const int SCREEN_WIDTH = 910;
const int SCREEN_HEIGHT = 750;

const int arraySize = 130;
const int barWidth = 7;

int array[arraySize];
int backupArray[arraySize];

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

bool isSorted = false;

bool initializeSDL()
{
    bool success = true;
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        cout << "SDL Initialization failed. SDL_Error: " << SDL_GetError();
        success = false;
    }
    else
    {
        if (!(SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")))
        {
            cout << "Warning: Linear texture filtering not enabled.\n";
        }

        window = SDL_CreateWindow("Sorting Visualizer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (window == NULL)
        {
            cout << "Window creation failed. SDL_Error: " << SDL_GetError();
            success = false;
        }
        else
        {
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
            if (renderer == NULL)
            {
                cout << "Renderer creation failed. SDL_Error: " << SDL_GetError();
                success = false;
            }
        }
    }

    return success;
}

void cleanup()
{
    SDL_DestroyRenderer(renderer);
    renderer = NULL;

    SDL_DestroyWindow(window);
    window = NULL;

    SDL_Quit();
}

void renderArray(int highlight1 = -1, int highlight2 = -1, int highlight3 = -1)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    int index = 0;
    for (int i = 0; i <= SCREEN_WIDTH - barWidth; i += barWidth)
    {
        SDL_PumpEvents();

        SDL_Rect rect = {i, 0, barWidth, array[index]};
        if (isSorted)
        {
            SDL_SetRenderDrawColor(renderer, 100, 180, 100, 0);
            SDL_RenderDrawRect(renderer, &rect);
        }
        else if (index == highlight1 || index == highlight3)
        {
            SDL_SetRenderDrawColor(renderer, 100, 180, 100, 0);
            SDL_RenderFillRect(renderer, &rect);
        }
        else if (index == highlight2)
        {
            SDL_SetRenderDrawColor(renderer, 165, 105, 189, 0);
            SDL_RenderFillRect(renderer, &rect);
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, 170, 183, 184, 0);
            SDL_RenderDrawRect(renderer, &rect);
        }
        index++;
    }
    SDL_RenderPresent(renderer);
}

void heapSort(int* input, int size)
{
    for (int i = 1; i < size; i++)
    {
        int childIndex = i;
        int parentIndex = (childIndex - 1) / 2;

        while (childIndex > 0)
        {
            if (input[childIndex] > input[parentIndex])
            {
                swap(input[parentIndex], input[childIndex]);
            }
            else
            {
                break;
            }

            renderArray(parentIndex, childIndex);
            SDL_Delay(40);

            childIndex = parentIndex;
            parentIndex = (childIndex - 1) / 2;
        }
    }

    for (int lastIndex = size - 1; lastIndex >= 0; lastIndex--)
    {
        swap(input[0], input[lastIndex]);

        int parentIndex = 0;
        int leftChildIndex = 2 * parentIndex + 1;
        int rightChildIndex = 2 * parentIndex + 2;

        while (leftChildIndex < lastIndex)
        {
            int largestIndex = parentIndex;

            if (input[leftChildIndex] > input[largestIndex])
            {
                largestIndex = leftChildIndex;
            }
            if (rightChildIndex < lastIndex && input[rightChildIndex] > input[largestIndex])
            {
                largestIndex = rightChildIndex;
            }
            if (largestIndex == parentIndex)
            {
                break;
            }

            swap(input[parentIndex], input[largestIndex]);

            renderArray(largestIndex, parentIndex, lastIndex);
            SDL_Delay(40);

            parentIndex = largestIndex;
            leftChildIndex = 2 * parentIndex + 1;
            rightChildIndex = 2 * parentIndex + 2;
        }
    }
}

int partition(int a[], int start, int end)
{
    int smallerCount = 0;

    for (int i = start + 1; i <= end; i++)
    {
        if (a[i] <= a[start])
        {
            smallerCount++;
        }
    }
    int pivotIndex = start + smallerCount;
    swap(a[pivotIndex], a[start]);
    renderArray(pivotIndex, start);

    int i = start, j = end;

    while (i < pivotIndex && j > pivotIndex)
    {
        if (a[i] <= a[pivotIndex])
        {
            i++;
        }
        else if (a[j] > a[pivotIndex])
        {
            j--;
        }
        else
        {
            swap(a[j], a[i]);

            renderArray(i, j);
            SDL_Delay(70);

            i++;
            j--;
        }
    }
    return pivotIndex;
}

void quickSort(int a[], int start, int end)
{
    if (start >= end)
    {
        return;
    }

    int pivotIndex = partition(a, start, end);
    quickSort(a, start, pivotIndex - 1);
    quickSort(a, pivotIndex + 1, end);
}

void mergeSortedArrays(int a[], int start, int end)
{
    int size_output = (end - start) + 1;
    int* output = new int[size_output];

    int mid = (start + end) / 2;
    int i = start, j = mid + 1, k = 0;
    while (i <= mid && j <= end)
    {
        if (a[i] <= a[j])
        {
            output[k] = a[i];
            renderArray(i, j);
            i++;
            k++;
        }
        else
        {
            output[k] = a[j];
            renderArray(i, j);
            j++;
            k++;
        }
    }
    while (i <= mid)
    {
        output[k] = a[i];
        renderArray(-1, i);
        i++;
        k++;
    }
    while (j <= end)
    {
        output[k] = a[j];
        renderArray(-1, j);
        j++;
        k++;
    }
    int x = 0;
    for (int l = start; l <= end; l++)
    {
        a[l] = output[x];
        renderArray(l);
        SDL_Delay(15);
        x++;
    }
    delete[] output;
}

void mergeSort(int a[], int start, int end)
{
    if (start >= end)
    {
        return;
    }
    int mid = (start + end) / 2;

    mergeSort(a, start, mid);
    mergeSort(a, mid + 1, end);

    mergeSortedArrays(a, start, end);
}

void bubbleSort()
{
    for (int i = 0; i < arraySize - 1; i++)
    {
        for (int j = 0; j < arraySize - 1 - i; j++)
        {
            if (array[j + 1] < array[j])
            {
                swap(array[j], array[j + 1]);

                renderArray(j + 1, j, arraySize - i);
            }
            SDL_Delay(1);
        }
    }
}

void insertionSort()
{
    for (int i = 1; i < arraySize; i++)
    {
        int j = i - 1;
        int temp = array[i];
        while (j >= 0 && array[j] > temp)
        {
            array[j + 1] = array[j];
            j--;

            renderArray(i, j + 1);
            SDL_Delay(5);
        }
        array[j + 1] = temp;
    }
}

void selectionSort()
{
    int minIndex;
    for (int i = 0; i < arraySize - 1; i++)
    {
        minIndex = i;
        for (int j = i + 1; j < arraySize; j++)
        {
            if (array[j] < array[minIndex])
            {
                minIndex = j;
                renderArray(i, minIndex);
            }
            SDL_Delay(1);
        }
        swap(array[i], array[minIndex]);
    }
}

void resetArray()
{
    memcpy(array, backupArray, sizeof(int) * arraySize);
}

void generateRandomArray()
{
    srand((unsigned)time(NULL));
    for (int i = 0; i < arraySize; i++)
    {
        int randomValue = rand() % (SCREEN_HEIGHT);
        backupArray[i] = randomValue;
    }
}

void run()
{
    if (!initializeSDL())
    {
        cout << "Failed to initialize.\n";
    }
    else
    {
        generateRandomArray();
        bool quit = false;
        SDL_Event event;
        while (!quit)
        {
            while (SDL_PollEvent(&event) != 0)
            {
                if (event.type == SDL_QUIT)
                {
                    quit = true;
                }
                else if (event.type == SDL_KEYDOWN)
                {
                    switch (event.key.keysym.sym)
                    {
                    case SDLK_h:
                        isSorted = false;
                        resetArray();
                        heapSort(array, arraySize);
                        isSorted = true;
                        break;
                    case SDLK_q:
                        isSorted = false;
                        resetArray();
                        quickSort(array, 0, arraySize - 1);
                        isSorted = true;
                        break;
                    case SDLK_m:
                        isSorted = false;
                        resetArray();
                        mergeSort(array, 0, arraySize - 1);
                        isSorted = true;
                        break;
                    case SDLK_b:
                        isSorted = false;
                        resetArray();
                        bubbleSort();
                        isSorted = true;
                        break;
                    case SDLK_i:
                        isSorted = false;
                        resetArray();
                        insertionSort();
                        isSorted = true;
                        break;
                    case SDLK_s:
                        isSorted = false;
                        resetArray();
                        selectionSort();
                        isSorted = true;
                        break;
                    }
                }
            }
            renderArray();
            SDL_Delay(100);
        }
    }

    cleanup();
}

int main(int argc, char* args[])
{
    run();
    return 0;
}


























// #include <SDL2/SDL.h>
// #include <iostream>
// #include <limits>
// #include <ctime>
// #include <cstring> // For memcpy
// #include <string>

// using namespace std;

// const int SCREEN_WIDTH = 910;
// const int SCREEN_HEIGHT = 750;

// const int arrSize = 130;
// const int rectSize = 7;

// int arr[arrSize];
// int Barr[arrSize];

// SDL_Window* window = NULL;
// SDL_Renderer* renderer = NULL;

// bool complete = false;

// bool init()
// {
//     bool success = true;
//     if (SDL_Init(SDL_INIT_VIDEO) < 0)
//     {
//         cout << "Couldn't initialize SDL. SDL_Error: " << SDL_GetError();
//         success = false;
//     }
//     else
//     {
//         if (!(SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")))
//         {
//             cout << "Warning: Linear Texture Filtering not enabled.\n";
//         }

//         window = SDL_CreateWindow("Sorting Visualizer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
//         if (window == NULL)
//         {
//             cout << "Couldn't create window. SDL_Error: " << SDL_GetError();
//             success = false;
//         }
//         else
//         {
//             renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
//             if (renderer == NULL)
//             {
//                 cout << "Couldn't create renderer. SDL_Error: " << SDL_GetError();
//                 success = false;
//             }
//         }
//     }

//     return success;
// }

// void close()
// {
//     SDL_DestroyRenderer(renderer);
//     renderer = NULL;

//     SDL_DestroyWindow(window);
//     window = NULL;

//     SDL_Quit();
// }

// void visualize(int x = -1, int y = -1, int z = -1)
// {
//     SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
//     SDL_RenderClear(renderer);

//     int j = 0;
//     for (int i = 0; i <= SCREEN_WIDTH - rectSize; i += rectSize)
//     {
//         SDL_PumpEvents();

//         SDL_Rect rect = {i, 0, rectSize, arr[j]};
//         if (complete)
//         {
//             SDL_SetRenderDrawColor(renderer, 100, 180, 100, 0);
//             SDL_RenderDrawRect(renderer, &rect);
//         }
//         else if (j == x || j == z)
//         {
//             SDL_SetRenderDrawColor(renderer, 100, 180, 100, 0);
//             SDL_RenderFillRect(renderer, &rect);
//         }
//         else if (j == y)
//         {
//             SDL_SetRenderDrawColor(renderer, 165, 105, 189, 0);
//             SDL_RenderFillRect(renderer, &rect);
//         }
//         else
//         {
//             SDL_SetRenderDrawColor(renderer, 170, 183, 184, 0);
//             SDL_RenderDrawRect(renderer, &rect);
//         }
//         j++;
//     }
//     SDL_RenderPresent(renderer);
// }

// void inplaceHeapSort(int* input, int n)
// {
//     for (int i = 1; i < n; i++)
//     {
//         int childIndex = i;
//         int parentIndex = (childIndex - 1) / 2;

//         while (childIndex > 0)
//         {
//             if (input[childIndex] > input[parentIndex])
//             {
//                 swap(input[parentIndex], input[childIndex]);
//             }
//             else
//             {
//                 break;
//             }

//             visualize(parentIndex, childIndex);
//             SDL_Delay(40);

//             childIndex = parentIndex;
//             parentIndex = (childIndex - 1) / 2;
//         }
//     }

//     for (int heapLast = n - 1; heapLast >= 0; heapLast--)
//     {
//         swap(input[0], input[heapLast]);

//         int parentIndex = 0;
//         int leftChildIndex = 2 * parentIndex + 1;
//         int rightChildIndex = 2 * parentIndex + 2;

//         while (leftChildIndex < heapLast)
//         {
//             int maxIndex = parentIndex;

//             if (input[leftChildIndex] > input[maxIndex])
//             {
//                 maxIndex = leftChildIndex;
//             }
//             if (rightChildIndex < heapLast && input[rightChildIndex] > input[maxIndex])
//             {
//                 maxIndex = rightChildIndex;
//             }
//             if (maxIndex == parentIndex)
//             {
//                 break;
//             }

//             swap(input[parentIndex], input[maxIndex]);

//             visualize(maxIndex, parentIndex, heapLast);
//             SDL_Delay(40);

//             parentIndex = maxIndex;
//             leftChildIndex = 2 * parentIndex + 1;
//             rightChildIndex = 2 * parentIndex + 2;
//         }
//     }
// }

// int partition_array(int a[], int si, int ei)
// {
//     int count_small = 0;

//     for (int i = (si + 1); i <= ei; i++)
//     {
//         if (a[i] <= a[si])
//         {
//             count_small++;
//         }
//     }
//     int c = si + count_small;
//     swap(a[c], a[si]);
//     visualize(c, si);

//     int i = si, j = ei;

//     while (i < c && j > c)
//     {
//         if (a[i] <= a[c])
//         {
//             i++;
//         }
//         else if (a[j] > a[c])
//         {
//             j--;
//         }
//         else
//         {
//             swap(a[j], a[i]);

//             visualize(i, j);
//             SDL_Delay(70);

//             i++;
//             j--;
//         }
//     }
//     return c;
// }

// void quickSort(int a[], int si, int ei)
// {
//     if (si >= ei)
//     {
//         return;
//     }

//     int c = partition_array(a, si, ei);
//     quickSort(a, si, c - 1);
//     quickSort(a, c + 1, ei);
// }

// void merge2SortedArrays(int a[], int si, int ei)
// {
//     int size_output = (ei - si) + 1;
//     int* output = new int[size_output];

//     int mid = (si + ei) / 2;
//     int i = si, j = mid + 1, k = 0;
//     while (i <= mid && j <= ei)
//     {
//         if (a[i] <= a[j])
//         {
//             output[k] = a[i];
//             visualize(i, j);
//             i++;
//             k++;
//         }
//         else
//         {
//             output[k] = a[j];
//             visualize(i, j);
//             j++;
//             k++;
//         }
//     }
//     while (i <= mid)
//     {
//         output[k] = a[i];
//         visualize(-1, i);
//         i++;
//         k++;
//     }
//     while (j <= ei)
//     {
//         output[k] = a[j];
//         visualize(-1, j);
//         j++;
//         k++;
//     }
//     int x = 0;
//     for (int l = si; l <= ei; l++)
//     {
//         a[l] = output[x];
//         visualize(l);
//         SDL_Delay(15);
//         x++;
//     }
//     delete[] output;
// }

// void mergeSort(int a[], int si, int ei)
// {
//     if (si >= ei)
//     {
//         return;
//     }
//     int mid = (si + ei) / 2;

//     mergeSort(a, si, mid);
//     mergeSort(a, mid + 1, ei);

//     merge2SortedArrays(a, si, ei);
// }

// void bubbleSort()
// {
//     for (int i = 0; i < arrSize - 1; i++)
//     {
//         for (int j = 0; j < arrSize - 1 - i; j++)
//         {
//             if (arr[j + 1] < arr[j])
//             {
//                 swap(arr[j], arr[j + 1]);

//                 visualize(j + 1, j, arrSize - i);
//             }
//             SDL_Delay(1);
//         }
//     }
// }

// void insertionSort()
// {
//     for (int i = 1; i < arrSize; i++)
//     {
//         int j = i - 1;
//         int temp = arr[i];
//         while (j >= 0 && arr[j] > temp)
//         {
//             arr[j + 1] = arr[j];
//             j--;

//             visualize(i, j + 1);
//             SDL_Delay(5);
//         }
//         arr[j + 1] = temp;
//     }
// }

// void selectionSort()
// {
//     int minIndex;
//     for (int i = 0; i < arrSize - 1; i++)
//     {
//         minIndex = i;
//         for (int j = i + 1; j < arrSize; j++)
//         {
//             if (arr[j] < arr[minIndex])
//             {
//                 minIndex = j;
//                 visualize(i, minIndex);
//             }
//             SDL_Delay(1);
//         }
//         swap(arr[i], arr[minIndex]);
//     }
// }

// void loadArr()
// {
//     memcpy(arr, Barr, sizeof(int) * arrSize);
// }

// void randomizeAndSaveArray()
// {
//     unsigned int seed = (unsigned)time(NULL);
//     srand(seed);
//     for (int i = 0; i < arrSize; i++)
//     {
//         int random = rand() % (SCREEN_HEIGHT);
//         Barr[i] = random;
//     }
// }

// void execute()
// {
//     if (!init())
//     {
//         cout << "SDL Initialization Failed.\n";
//     }
//     else
//     {
//         randomizeAndSaveArray();
//         loadArr();

//         SDL_Event e;
//         bool quit = false;
//         while (!quit)
//         {
//             while (SDL_PollEvent(&e) != 0)
//             {
//                 if (e.type == SDL_QUIT)
//                 {
//                     quit = true;
//                     complete = false;
//                 }
//                 else if (e.type == SDL_KEYDOWN)
//                 {
//                     switch (e.key.keysym.sym)
//                     {
//                         case (SDLK_q):
//                             quit = true;
//                             complete = false;
//                             cout << "\nEXITING SORTING VISUALIZER.\n";
//                             break;
//                         case (SDLK_0):
//                             randomizeAndSaveArray();
//                             complete = false;
//                             loadArr();
//                             cout << "\nNEW RANDOM LIST GENERATED.\n";
//                             break;
//                         case (SDLK_1):
//                             loadArr();
//                             cout << "\nSELECTION SORT STARTED.\n";
//                             complete = false;
//                             selectionSort();
//                             complete = true;
//                             cout << "\nSELECTION SORT COMPLETE.\n";
//                             break;
//                         case (SDLK_2):
//                             loadArr();
//                             cout << "\nINSERTION SORT STARTED.\n";
//                             complete = false;
//                             insertionSort();
//                             complete = true;
//                             cout << "\nINSERTION SORT COMPLETE.\n";
//                             break;
//                         case (SDLK_3):
//                             loadArr();
//                             cout << "\nBUBBLE SORT STARTED.\n";
//                             complete = false;
//                             bubbleSort();
//                             complete = true;
//                             cout << "\nBUBBLE SORT COMPLETE.\n";
//                             break;
//                         case (SDLK_4):
//                             loadArr();
//                             cout << "\nMERGE SORT STARTED.\n";
//                             complete = false;
//                             mergeSort(arr, 0, arrSize - 1);
//                             complete = true;
//                             cout << "\nMERGE SORT COMPLETE.\n";
//                             break;
//                         case (SDLK_5):
//                             loadArr();
//                             cout << "\nQUICK SORT STARTED.\n";
//                             complete = false;
//                             quickSort(arr, 0, arrSize - 1);
//                             complete = true;
//                             cout << "\nQUICK SORT COMPLETE.\n";
//                             break;
//                         case (SDLK_6):
//                             loadArr();
//                             cout << "\nHEAP SORT STARTED.\n";
//                             complete = false;
//                             inplaceHeapSort(arr, arrSize);
//                             complete = true;
//                             cout << "\nHEAP SORT COMPLETE.\n";
//                             break;
//                     }
//                 }
//             }
//             visualize();
//         }
//         close();
//     }
// }

// bool controls()
// {
//     cout << "WARNING: Giving repetitive commands may cause latency and the visualizer may behave unexpectedly. Please give a new command only after the current command's execution is done.\n\n"
//          << "Available Controls inside Sorting Visualizer:-\n"
//          << "    Use 0 to Generate a different randomized list.\n"
//          << "    Use 1 to start Selection Sort Algorithm.\n"
//          << "    Use 2 to start Insertion Sort Algorithm.\n"
//          << "    Use 3 to start Bubble Sort Algorithm.\n"
//          << "    Use 4 to start Merge Sort Algorithm.\n"
//          << "    Use 5 to start Quick Sort Algorithm.\n"
//          << "    Use 6 to start Heap Sort Algorithm.\n"
//          << "    Use q to exit out of Sorting Visualizer\n\n"
//          << "PRESS ENTER TO START SORTING VISUALIZER...\n\n"
//          << "Or type -1 and press ENTER to quit the program.";

//     string s;
//     getline(cin, s);
//     if (s == "-1")
//     {
//         return false;
//     }
//     return true;
// }

// void intro()
// {
//     cout << "==============================Sorting Visualizer==============================\n\n"
//          << "Visualization of different sorting algorithms in C++ with SDL2 Library. A sorting algorithm is an algorithm that puts the elements of a list in a certain order. While there are a large number of sorting algorithms, in practical implementations a few algorithms predominate.\n"
//          << "In this implementation of sorting visualizer, we'll be looking at some of these sorting algorithms and visually comprehend their working.\n"
//          << "The sorting algorithms covered here are Selection Sort, Insertion Sort, Bubble Sort, Merge Sort, Quick Sort and Heap Sort.\n"
//          << "The list size is fixed to 130 elements. You can randomize the list and select any type of sorting algorithm to call on the list from the given options. Here, all sorting algorithms will sort the elements in ascending order. The sorting time being visualized for an algorithm is not exactly same as their actual time complexities. The relatively faster algorithms like Merge Sort, etc. have been delayed so that they could be properly visualized.\n\n"
//          << "Press ENTER to show controls...";

//     string s;
//     getline(cin, s);
//     if (s == "\n")
//     {
//         return;
//     }
// }

// int main(int argc, char* args[])
// {
//     intro();

//     while (1)
//     {
//         cout << '\n';
//         if (controls())
//             execute();
//         else
//         {
//             cout << "\nEXITING PROGRAM.\n";
//             break;
//         }
//     }

//     return 0;
// }
// // g++ -I src/include -L src/lib -o main main.cpp -lmingw32 -lSDL2main -lSDL2
// //  #include <SDL2/SDL.h>
// // #include<iostream>
// // #include<ctime>
// // #include<iomanip>
// // using namespace std;

// // //Screen dimension constants
// // const int WIDTH = 1000;
// // const int HEIGHT = 600;
// // const int SIZE = 500;
// // SDL_Window* window = NULL;
// // SDL_Renderer* renderer;
// // SDL_Event event;
// // bool quit = false;

// // void GenerateArray(int* arr);
// // void Swap(int* arr, int i, int j);
// // void SelectionSort(int* arr);
// // void PrintArray(int* arr);
// // void draw(int* arr, int i, int j);

// // void setup()
// // {
// // 	//Create window
// // 	window = SDL_CreateWindow("Selection Sort", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
// // 	if (window == NULL)
// // 	{
// // 		std::cout << "Window could not be created! SDL_Error" << std::endl << SDL_GetError();
// // 	}
// // 	else
// // 	{
// // 		//Get window surface
// // 		renderer = SDL_CreateRenderer(window, -1, 0);

// // 		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

// // 		SDL_RenderClear(renderer);

// // 		//Update the surface
// // 		SDL_UpdateWindowSurface(window);
// // 	}
// // }
// // bool processEvent()
// // {
// // 	while (SDL_PollEvent(&event))
// // 	{
// // 		switch (event.type)
// // 		{
// // 			// if window closed then quit the game
// // 		case SDL_WINDOWEVENT_CLOSE:
// // 		{
// // 			if (window)
// // 			{
// // 				SDL_DestroyWindow(window);
// // 				window = NULL;
// // 				quit = true;
// // 			}
// // 		}
// // 		break;
// // 		// if escape keyboard passed then quit the game
// // 		case SDL_KEYDOWN:
// // 		{
// // 			switch (event.key.keysym.sym)
// // 			{
// // 			case SDLK_ESCAPE:
// // 				quit = true;
// // 				break;
// // 			}
// // 		}
// // 		break;
// // 		// quit the game
// // 		case SDL_QUIT:
// // 			quit = 1;
// // 			break;
// // 		}
// // 	}
// // 	return quit;
// // }


// // void GenerateArray(int* arr)
// // {
// // 	srand(time(NULL));
// // 	for (int i = 0; i < SIZE; i++)
// // 	{
// // 		arr[i] = 1 + rand() % HEIGHT;
// // 	}
// // }
// // // swapping i and j elements in array
// // void Swap(int* arr, int i, int j)
// // {
// // 	int temp = arr[i];
// // 	arr[i] = arr[j];
// // 	arr[j] = temp;
// // }
// // void SelectionSort(int* arr)
// // {
// // 	int i, j, index;
// // 	for (i = 0; i < SIZE - 1; i++)
// // 	{
// // 		index = i;
// // 		for (j = i + 1; j < SIZE; j++)
// // 		{
// // 			if (arr[j] < arr[index]) {
// // 				index = j;
// // 			}
// // 		}
// // 		draw(arr, i, index);
// // 		Swap(arr, i, index);
// // 		SDL_Delay(5);
// // 	}
// // 	SDL_Rect rect;
// // 	SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
// // 	rect = { (SIZE - 1) * (WIDTH / SIZE),HEIGHT - arr[SIZE - 1],WIDTH / SIZE,HEIGHT };
// // 	SDL_RenderFillRect(renderer, &rect);
// // 	SDL_RenderPresent(renderer);
// // }
// // void PrintArray(int* arr)
// // {
// // 	for (int i = 0; i < SIZE; i++)
// // 	{
// // 		cout << arr[i] << ' ';
// // 	}
// // }
// // void draw(int* arr, int i, int j)
// // {
// // 	SDL_Rect rect;

// // 	SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
// // 	rect = { j * WIDTH / SIZE,HEIGHT - arr[j],WIDTH / SIZE,HEIGHT };
// // 	SDL_RenderFillRect(renderer, &rect);
// // 	SDL_RenderPresent(renderer);

// // 	SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
// // 	rect = { i * WIDTH / SIZE,HEIGHT - arr[i],WIDTH / SIZE,HEIGHT };
// // 	SDL_RenderFillRect(renderer, &rect);
// // 	SDL_RenderPresent(renderer);

// // 	//SDL_Delay(50);

// // 	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
// // 	rect = { j * WIDTH / SIZE,HEIGHT - arr[j],WIDTH / SIZE,HEIGHT };
// // 	SDL_RenderFillRect(renderer, &rect);

// // 	SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255);
// // 	rect = { j * WIDTH / SIZE,HEIGHT - arr[i],WIDTH / SIZE,HEIGHT };
// // 	SDL_RenderFillRect(renderer, &rect);
// // 	SDL_RenderPresent(renderer);


// // 	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
// // 	rect = { i * WIDTH / SIZE,HEIGHT - arr[i],WIDTH / SIZE,HEIGHT };
// // 	SDL_RenderFillRect(renderer, &rect);

// // 	SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
// // 	rect = { i * WIDTH / SIZE,HEIGHT - arr[j],WIDTH / SIZE,HEIGHT };
// // 	SDL_RenderFillRect(renderer, &rect);
// // 	SDL_RenderPresent(renderer);

// // }
// // int main(int argc, char* args[])
// // {
// // 	time_t start, end;
// // 	int arr[SIZE];
// // 	GenerateArray(arr);

// // 	if (SDL_Init(SDL_INIT_VIDEO) < 0)
// // 	{
// // 		std::cout<<"SDL could not initialize! SDL_Error" << std::endl << SDL_GetError();
// // 	}
// // 	else setup();


// // 	SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255);
// // 	for (int i = 0; i < SIZE; i++)
// // 	{
// // 		SDL_Rect rect = { i * WIDTH / SIZE,HEIGHT - arr[i],WIDTH / SIZE,HEIGHT };
// // 		SDL_RenderFillRect(renderer, &rect);
// // 		SDL_RenderPresent(renderer);
// // 	}
// // 	time(&start);
// // 	SelectionSort(arr);
// // 	time(&end);
// // 	// Calculating total time taken by the program. 
// // 	double time_taken = double(end - start);
// // 	std::cout << "Array size: " << SIZE << std::endl;
// // 	std::cout << "Time taken by program is : " << std::fixed << time_taken << std::setprecision(5);
// // 	std::cout << " sec " << std::endl;
// // 	while (!quit)
// // 	{
// // 		quit = processEvent();
// // 	}

// // 	//Destroy renderer
// // 	SDL_DestroyRenderer(renderer);
// // 	//Destroy window
// // 	SDL_DestroyWindow(window);
// // 	//Quit SDL subsystems
// // 	SDL_Quit();

// // 	return 0;
// // }

