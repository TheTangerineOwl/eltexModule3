#ifndef CONTACTS_H
#define CONTACTS_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#define FILENAME "contacts.bin"

#define LAST_NAME_LENGTH 50
#define FIRST_NAME_LENGTH 50
#define PATRONIM_LENGTH 50
#define COMPANY_NAME_LENGTH 256
#define POSITION_LENGTH 256
#define MAX_EMAIL_LENGTH 256
#define PHONE_NUMBER_LENGTH 12
#define SOCIALS_LINK_LENGTH 256
#define MAX_SOCIALS_COUNT 5
#define MAX_EMAILS_COUNT 5
#define MAX_CONTACTS_COUNT 100

typedef struct
{
    char workPlace[COMPANY_NAME_LENGTH];
    char position[POSITION_LENGTH];
} Work_t;

typedef struct
{
    char work[PHONE_NUMBER_LENGTH];
    char personal[PHONE_NUMBER_LENGTH];
    char home[PHONE_NUMBER_LENGTH];
    char extra[PHONE_NUMBER_LENGTH];
} PhoneNumbers_t;

typedef struct
{
    char lastName[LAST_NAME_LENGTH];
    char firstName[FIRST_NAME_LENGTH];
    char patronim[PATRONIM_LENGTH];
    PhoneNumbers_t numbers;
    Work_t workInfo;
    char emails[MAX_EMAILS_COUNT][MAX_EMAIL_LENGTH];
    char socialsLink[MAX_SOCIALS_COUNT][SOCIALS_LINK_LENGTH];
} Contact_t;

extern Contact_t contacts[MAX_CONTACTS_COUNT];
extern size_t contactsCount;

Contact_t* clearContact(Contact_t* contact);
Contact_t* addContact(
    Contact_t* contacts,
    const char* lastName,
    const char* firstName,
    const char* patronim,
    const char* workPlace,
    const char* position,
    const char* workPhone,
    const char* personalPhone,
    const char* homePhone,
    const char* extraPhone,
    char emails[MAX_EMAILS_COUNT][MAX_EMAIL_LENGTH],
    char socials[MAX_SOCIALS_COUNT][SOCIALS_LINK_LENGTH] 
);

int deleteContact(
    Contact_t* contacts,
    size_t index
);

Contact_t* editContact(
    Contact_t* contact,
    const char* lastName,
    const char* firstName,
    const char* patronim,
    const char* workPlace,
    const char* position,
    const char* workPhone,
    const char* personalPhone,
    const char* homePhone,
    const char* extraPhone,
    char emails[MAX_EMAILS_COUNT][MAX_EMAIL_LENGTH],
    char socials[MAX_SOCIALS_COUNT][SOCIALS_LINK_LENGTH]
);

Contact_t* copyContactInfo(
    Contact_t* contact,
    const char* lastName,
    const char* firstName,
    const char* patronim,
    const char* workPlace,
    const char* position,
    const char* workPhone,
    const char* personalPhone,
    const char* homePhone,
    const char* extraPhone,
    char emails[MAX_EMAILS_COUNT][MAX_EMAIL_LENGTH],
    char socials[MAX_SOCIALS_COUNT][SOCIALS_LINK_LENGTH]  
);

typedef struct
{
    const char* name;
    const size_t fieldLength;
    const size_t itemCount;
} ContactField;

enum ContactFieldType
{
    LAST_NAME, FIRST_NAME, PATRONIM,
    WORK_PLACE, POSITION,
    WORK_PHONE, PERSONAL_PHONE, HOME_PHONE, EXTRA_PHONE,
    EMAIL, SOCIAL_LINK
};

static const ContactField menuItems[] = {
    {"Фамилия", LAST_NAME_LENGTH, 1},
    {"Имя", FIRST_NAME_LENGTH, 1},
    {"Отчество", PATRONIM_LENGTH, 1},
    {"Место работы", COMPANY_NAME_LENGTH, 1},
    {"Должность", POSITION_LENGTH, 1},
    {"Рабочий телефон", PHONE_NUMBER_LENGTH, 1},
    {"Личный телефон", PHONE_NUMBER_LENGTH, 1},
    {"Домашний телефон", PHONE_NUMBER_LENGTH, 1},
    {"Дополнительный телефон", PHONE_NUMBER_LENGTH, 1},
    {"Электронная почта", MAX_EMAIL_LENGTH, MAX_EMAILS_COUNT},
    {"Ссылка на соцсеть", SOCIALS_LINK_LENGTH, MAX_SOCIALS_COUNT}
};

static const size_t menuCount = sizeof(menuItems) / sizeof(ContactField);

char* enterStrField(char* field, size_t fieldLength);
void printContact(const Contact_t contact);
void printAllContacts(const Contact_t* contacts);
int enterContactInfo(Contact_t* contacts);
int editContactInfo(Contact_t *contact);
char* changeField(char* oldField, const ContactField field);
void printShortContacts(const Contact_t* contacts);
Contact_t* chooseContact(Contact_t* contacts);

ssize_t readContacts(const char* filename);
ssize_t writeContacts(const char* filename);

#endif