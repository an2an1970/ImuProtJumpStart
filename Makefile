# Имя исполняемого файла
TARGET = ImuProtExample

# Компилятор и флаги
CC = gcc
CFLAGS = -Wall -Wextra -std=c11

# Исходные файлы
SRCS = ImuProtExample.c

# Объектные файлы
OBJS = $(SRCS:.c=.o)

# Правила

# Правило по умолчанию
all: $(TARGET)

# Правило для создания исполняемого файла
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Правило для компиляции исходных файлов в объектные файлы
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Правило для очистки сгенерированных файлов
clean:
	rm -f $(TARGET) $(OBJS)

# Правило для удаления всех файлов, кроме исходных
distclean: clean
	rm -f *~ *.bak

# Правило для создания справки
help:
	@echo "Makefile commands:"
	@echo "  all       - Build the project"
	@echo "  clean     - Remove generated files"
	@echo "  distclean - Remove all generated files and backups"
	@echo "  help      - Show this help message"