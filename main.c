#include <stdio.h>
#include <time.h>
#include <tesseract/capi.h>
#include <leptonica/allheaders.h>
#include <pthread.h>
#include "main.h"

#define N 14

//инициализируем структуру, т.к. только один объект передается в поток
typedef struct {
    TessBaseAPI *api;
    struct Pix *image;
    int count;
} pthread_data;

void * recognize(void * thread_data) {
//    приводим к типу заданной структуры
    pthread_data *data = (pthread_data *) thread_data;
    TessBaseAPISetImage2(data->api, data->image);
    char *result = TessBaseAPIGetUTF8Text(data->api);
    printf("%d - %s", data->count, result);
    TessBaseAPIEnd(data->api);
    TessBaseAPIDelete(data->api);
    TessDeleteText(result);
    pthread_exit(0);
}

int main() {
    struct Pix *image = pixRead("./img0.png");

//    выделяем память под массив идентификаторов потоков
    pthread_t* threads = (pthread_t*) malloc(N * sizeof(pthread_t));
//    сколько потоков - столько и структур с потоковых данных
    pthread_data *threadData = (pthread_data *) malloc(N * sizeof(pthread_data));

//    создаем массив со ссылками на api Тессеракта
    TessBaseAPI * apis[N];
    for (int i = 0; i < N; i++) {
        apis[i] = TessBaseAPICreate();
        TessBaseAPIInit2(apis[i], "./tessdata", "rus", OEM_TESSERACT_ONLY);
    }

    clock_t start = clock();
//    заполняем данными в цикле структуру и запускаем потом
    for (int i = 0; i < N; i++) {
        threadData[i].api = apis[i];
        threadData[i].image = image;
        threadData[i].count = i;

        pthread_create(&threads[i], NULL, recognize, &threadData[i]);
    }
//    ожидаем выполнение всех потоков
    for(int i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }
//    освобождаем память
    free(threads);
    free(threadData);

    clock_t duration = clock() - start;
    printf("Время: %lf сек.\n", (double) duration / CLOCKS_PER_SEC);

    pixDestroyColormap(image);

    return 0;
}
