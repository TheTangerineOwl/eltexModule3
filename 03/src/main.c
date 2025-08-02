#include "contacts.h"

Contact_t contacts[MAX_CONTACTS_COUNT];
size_t contactsCount = 0;

int main()
{
    // Contact_t contacts[MAX_CONTACTS_COUNT];
    
    ssize_t readFile = readContacts(FILENAME);
    if (readFile < 0)
        return EXIT_FAILURE;
    contactsCount = readFile;

    size_t choice;
    int exitFlag = 0;
    Contact_t* currentContact;

    char c;

    while (!exitFlag)
    {
        puts("\nВыберите опцию:");
        puts("1. Добавление контакта;");
        puts("2. Редактирование контакта;");
        puts("3. Удаление контакта;");
        puts("4. Вывод информации о контакте;");
        puts("5. Вывести все контакты;");
        printf("6. Выход. Ваш выбор: ");

        while (!scanf("%ld", &choice) || choice < 1 || choice > 6)
        {
            puts("Некорректный выбор! Попробуйте еще раз.\n");
            while ((c = getchar() != '\n') && c != EOF);
        }
        while ((c = getchar()) != '\n' && c != EOF);

        switch (choice)
        {
        case 1:
            enterContactInfo(contacts);
            break;
        case 2:
        {
            currentContact = chooseContact(contacts);
            if (!currentContact)
            {
                puts("Ошибка выбора контакта!");
                continue;
            }
            editContactInfo(contacts);
        }
        break;
        case 3:
        {
            if (!contactsCount)
            {
                puts("Нет контактов для удаления!");
                continue;
            }
            puts("Все контакты:");
            printShortContacts(contacts);
            printf("Выберите индекс контакта для удаления: ");
            size_t choice;
            while (!scanf("%ld", &choice) || choice < 1 || choice > contactsCount)
            {
                printf("Некорректный выбор! Попробуйте еще раз.\n");
                while ((c = getchar() != '\n') && c != EOF);
            }
            while ((c = getchar()) != '\n' && c != EOF);
            deleteContact(contacts, choice - 1);
        }
        break;
        case 4:
        {
            currentContact = chooseContact(contacts);
            if (!currentContact)
            {
                puts("Ошибка выбора контакта!");
                continue;
            }
            printContact(*currentContact);
        }
        break;
        case 5:
            printShortContacts(contacts);
            break;
        case 6:
            exitFlag = 1;
            break;
        default:
            printf("Некорректный выбор! Попробуйте еще раз.\n");
            break;
        }
    }

    ssize_t writeFile = writeContacts(FILENAME);
    if (writeFile < contactsCount)
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}