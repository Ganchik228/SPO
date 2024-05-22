#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#define DEVICE_PATH "/dev/sysmon"

int main() {
    int fd;
    char buffer[1024];

    // Открываем устройство
    fd = open(DEVICE_PATH, O_RDONLY);
    if (fd < 0) {
        perror("Ошибка открытия устройства");
        return -1;
    }

    // Читаем данные из устройства
    ssize_t bytes_read = read(fd, buffer, sizeof(buffer));
    if (bytes_read < 0) {
        perror("Ошибка чтения из устройства");
        close(fd);
        return -1;
    }

    // Выводим прочитанные данные
    printf("Данные из устройства:\n%s\n", buffer);

    // Закрываем устройство
    close(fd);

    return 0;
}
