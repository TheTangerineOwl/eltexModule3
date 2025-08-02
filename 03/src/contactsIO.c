#include "contacts.h"

#define CLEAR_BUFFER() \
    while ((c = getchar()) != '\n' && c != EOF)
#define PRINT_FIELD_IFNZERO(field, fieldName) \
    if (field && field[0] != 0) printf("%s: %s\n", fieldName, field)
char c;

char* enterStrField(char* field, size_t fieldLength)
{
    if (!fgets(field, fieldLength, stdin))
    {
        puts("Не удалось прочитать строку из потока ввода!");
        CLEAR_BUFFER();
        return NULL;
    }
    field[strcspn(field, "\n")] = '\0';
    if (strlen(field) == fieldLength - 1)
        CLEAR_BUFFER();
    return field;
}

void printContact(const Contact_t contact)
{
    printf("Фамилия: %s\n", contact.lastName);
    printf("Имя: %s\n", contact.firstName);
    PRINT_FIELD_IFNZERO(contact.patronim, "Отчество");
    PRINT_FIELD_IFNZERO(contact.workInfo.workPlace, "Место работы");
    PRINT_FIELD_IFNZERO(contact.workInfo.position, "Должность");
    PRINT_FIELD_IFNZERO(contact.numbers.work, "Рабочий телефон");
    PRINT_FIELD_IFNZERO(contact.numbers.personal, "Личный телефон");
    PRINT_FIELD_IFNZERO(contact.numbers.home, "Домашний телефон");
    PRINT_FIELD_IFNZERO(contact.numbers.extra, "Дополнительный телефон");
    if (strlen(contact.emails[0]) > 0)
    {
        printf("Электронные почты: \n");
        for (size_t i = 0; i < MAX_EMAILS_COUNT && strlen(contact.emails[i]) > 0; i++)
            printf("%ld. %s\n", i + 1, contact.emails[i]);
    }
    if (strlen(contact.socialsLink[0]) > 0)
    {
        printf("Социальные сети и мессенджеры: \n");
        for (size_t j = 0; j < MAX_EMAILS_COUNT && strlen(contact.socialsLink[j]) > 0; j++)
            printf("%ld. %s\n", j + 1, contact.socialsLink[j]);
    }
}

void printAllContacts(const Contact_t* contacts)
{
    if (contactsCount == 0)
        puts("Нет контактов!");
    for (size_t i = 0; i < contactsCount; i++)
    {
        printf("%ld. ", i + 1);
        printContact(contacts[i]);
    }
}

void printShortContacts(const Contact_t* contacts)
{
    if (contactsCount == 0)
        puts("Нет контактов!");
    for (size_t i = 0; i < contactsCount; i++)
    {
        printf("%ld. %s %s %s %s\n",
            i + 1,
            contacts[i].lastName, contacts[i].firstName, contacts[i].patronim,
            contacts[i].numbers.personal);
    }
}

int enterContactInfo(Contact_t* contacts)
{
    if (contactsCount + 1 > MAX_CONTACTS_COUNT)
    {
        printf("Нельзя добавить контакт!\n");
        return 0;
    }
    char lastName[LAST_NAME_LENGTH];
    char firstName[FIRST_NAME_LENGTH];
    char patronim[PATRONIM_LENGTH];
    char work[PHONE_NUMBER_LENGTH];
    char personal[PHONE_NUMBER_LENGTH];
    char home[PHONE_NUMBER_LENGTH];
    char extra[PHONE_NUMBER_LENGTH];
    char workPlace[COMPANY_NAME_LENGTH];
    char position[POSITION_LENGTH];
    char emails[MAX_EMAILS_COUNT][MAX_EMAIL_LENGTH];
    char socialsLink[MAX_SOCIALS_COUNT][SOCIALS_LINK_LENGTH];

    printf("Введите фамилию: ");
    enterStrField(lastName, LAST_NAME_LENGTH);
    while (strlen(lastName) == 0)
    {
        puts("Поле \"фамилия\" должно быть заполнено!");
        enterStrField(lastName, LAST_NAME_LENGTH);
    }
    printf("Введите имя: ");
    enterStrField(firstName, FIRST_NAME_LENGTH);
    while (strlen(firstName) == 0)
    {
        puts("Поле \"имя\" должно быть заполнено!");
        enterStrField(lastName, LAST_NAME_LENGTH);
    }

    printf("Введите отчество (пустая строка для пропуска): ");
    enterStrField(patronim, PATRONIM_LENGTH);
    printf("Введите место работы (пустая строка для пропуска): "); 
    enterStrField(workPlace, COMPANY_NAME_LENGTH);
    printf("Введите должность (пустая строка для пропуска): ");
    enterStrField(position, POSITION_LENGTH);
    printf("Введите рабочий телефон (пустая строка для пропуска): ");
    enterStrField(work, PHONE_NUMBER_LENGTH);
    printf("Введите личный телефон (пустая строка для пропуска): ");
    enterStrField(personal, PHONE_NUMBER_LENGTH);
    printf("Введите домашний телефон (пустая строка для пропуска): ");
    enterStrField(home, PHONE_NUMBER_LENGTH);
    printf("Введите дополнительный телефон (пустая строка для пропуска): ");
    enterStrField(extra, PHONE_NUMBER_LENGTH);
    for (size_t i = 0; i < MAX_EMAILS_COUNT; i++)
    {
        printf("Введите электронную почту %ld (пустая строка для пропуска): ", i + 1);
        enterStrField(emails[i], MAX_EMAIL_LENGTH);
        if (strlen(emails[i]) == 0)
            break;
    }
    for (size_t i = 0; i < MAX_SOCIALS_COUNT; i++)
    {
        printf("Введите ссылку на соцсети или профиль %ld (пустая строка для пропуска): ", i + 1);
        enterStrField(socialsLink[i], SOCIALS_LINK_LENGTH);
        if (strlen(socialsLink[i]) == 0)
            break;
    }

    Contact_t* contact = addContact(contacts,
        lastName, firstName, patronim,
        workPlace, position,
        work, personal, home, extra,
        emails, socialsLink
    );
    printContact(*contact);
    return 1;
}

char* changeField(char* oldField, const ContactField field)
{
    printf("Редактирование поля \"%s\".\n", field.name);
    printf("Старое значение: %s\n", oldField);
    printf("Введите новое значение: ");
    if (!enterStrField(oldField, field.fieldLength))
        return NULL;
    return oldField;
}

int editContactInfo(Contact_t *contact)
{
    Contact_t newValues = *contact;
    int exitFlag = 0;
    if (!contact)
    {
        puts("Не удалось редактировать контакт: контакт не найден!");
        return 0;
    }
    printf("Редактирование контакта:\n\n");
    printContact(*contact);
    int choice;
    while (!exitFlag)
    {
        printf("Выберите поле для редактирования:\n");

        for (size_t i = 0; i < menuCount; i++)
        {
            printf("%ld. %s\n", i + 1, menuItems[i].name);
        }
        printf("%ld. Выход из режима редактирования.\n Ваш выбор: ", menuCount + 1);

        if (!scanf("%d", &choice) || choice < 1 || (size_t)choice > menuCount + 1)
        {
            printf("Некорректный выбор! Попробуйте еще раз.\n");
            CLEAR_BUFFER();
            continue;
        }
        CLEAR_BUFFER();

        choice--;

        switch (choice)
        {
        case LAST_NAME:
            changeField(newValues.lastName, menuItems[LAST_NAME]);
            break;
        case FIRST_NAME:
            changeField(newValues.firstName, menuItems[FIRST_NAME]);
            break;
        case PATRONIM:
            changeField(newValues.patronim, menuItems[PATRONIM]);
            break;
        case WORK_PLACE:
            changeField(newValues.workInfo.workPlace, menuItems[WORK_PLACE]);
            break;
        case POSITION:
            changeField(newValues.workInfo.position, menuItems[POSITION]);
            break;
        case WORK_PHONE:
            changeField(newValues.numbers.work, menuItems[WORK_PHONE]);
            break;
        case PERSONAL_PHONE:
            changeField(newValues.numbers.personal, menuItems[PERSONAL_PHONE]);
            break;
        case HOME_PHONE:
            changeField(newValues.numbers.home, menuItems[HOME_PHONE]);
            break;
        case EXTRA_PHONE:
            changeField(newValues.numbers.extra, menuItems[EXTRA_PHONE]);
            break;
        case EMAIL:
        {
            char oldValue[MAX_EMAIL_LENGTH];
            for (size_t i = 0; i < MAX_EMAILS_COUNT; i++)
            {
                strcpy(oldValue, newValues.emails[i]);
                if (i > 1 && strlen(newValues.emails[i - 1]) < 1)
                    break;
                changeField(newValues.emails[i], menuItems[EMAIL]);
                if (strlen(newValues.emails[i]) < 1)
                    strcpy(newValues.emails[i], oldValue);
            }
        }
        break;
        case SOCIAL_LINK:
        {
            char oldValue[MAX_EMAIL_LENGTH];
            for (size_t i = 0; i < MAX_EMAILS_COUNT; i++)
            {
                strcpy(oldValue, newValues.socialsLink[i]);
                if (i > 1 && strlen(newValues.socialsLink[i - 1]) < 1)
                    break;
                changeField(newValues.socialsLink[i], menuItems[SOCIAL_LINK]);
                if (strlen(newValues.socialsLink[i]) < 1)
                    strcpy(newValues.socialsLink[i], oldValue);
            }
        }
        break;
        case SOCIAL_LINK + 1:
        {
            puts("Значения до изменения:");
            printContact(*contact);
            puts("После изменения:");
            printContact(newValues);
            printf("Закончить редактирование (0 - нет, 1 - да, 2 - отменить изменения)? Выбор: ");

            while (!scanf("%d", &choice) || choice < 0 || choice > 2)
            {
                printf("Некорректный выбор! Попробуйте еще раз.\n");
                CLEAR_BUFFER();
            }
            CLEAR_BUFFER();
            if (choice == 0)
                continue;
            if (choice == 1)
                *contact = newValues;
            exitFlag = 1;
        }
        break;
        default:
        {
            puts("Некорректный ввод! Попробуйте еще раз.");
            CLEAR_BUFFER();
            continue;
        }
        }
        puts("Новые значения:");
        printContact(newValues);
    }
    return 1;
}

Contact_t* chooseContact(Contact_t* contacts)
{
    puts("Все контакты:");
    printShortContacts(contacts);
    if (!contactsCount)
        return NULL;
    printf("Выберите индекс контакта: ");
    size_t choice;

    while (!scanf("%ld", &choice) || choice < 1 || choice > contactsCount)
    {
        printf("Некорректный выбор! Попробуйте еще раз.\n");
        CLEAR_BUFFER();
    }
    CLEAR_BUFFER();
    return &contacts[choice - 1];
}