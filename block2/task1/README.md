# Задача 2.1
Проблема конкурентного доступа к разделяемому ресурсу.
- a) В каталоге sync репозитория (...) вы найдете простую
реализацию очереди на списке. Изучите код, соберите и запустите программу
queue-example.c. Посмотрите вывод программы и убедитесь что он
соответствует вашему пониманию работы данной реализации очереди.
Добавьте реализацию функции queue_destroy().
- b) Изучите код программы queue-threads.c и разберитесь что она делает. Соберите
программу.
  - i.Запустите программу несколько раз. Если появляются ошибки
выполнения, попытайтесь их объяснить и определить что именно
вызывает ошибку. Какие именно ошибки вы наблюдали?
  - ii. Поиграйте следующими параметрами:
    - 1. размером очереди (задается в queue_init()). Запустите программуc размером очереди от 1000 до 1000000.
    - 2. привязкой к процессору (задается функцией set_cpu()). Привяжите
потоки к одному процессору (ядру) и к разным.
    - 3. планированием потоков (функция sched_yield()). Попробуйте
убрать эту функцию перед созданием в
