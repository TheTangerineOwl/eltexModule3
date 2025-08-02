#include "contacts.h"

Contact_t* clearContact(Contact_t* contact)
{
    if (!contact)
        return NULL;
    strncpy(contact->lastName, "", LAST_NAME_LENGTH - 1);
    strncpy(contact->firstName, "", FIRST_NAME_LENGTH - 1);
    strncpy(contact->patronim, "", PATRONIM_LENGTH - 1);
    strncpy(contact->workInfo.workPlace, "", COMPANY_NAME_LENGTH - 1);
    strncpy(contact->workInfo.position, "", POSITION_LENGTH - 1);
    strncpy(contact->numbers.work, "", PHONE_NUMBER_LENGTH - 1);
    strncpy(contact->numbers.personal, "", PHONE_NUMBER_LENGTH - 1);
    strncpy(contact->numbers.home, "", PHONE_NUMBER_LENGTH - 1);
    strncpy(contact->numbers.extra, "", PHONE_NUMBER_LENGTH - 1);
    for (size_t i = 0; i < MAX_EMAILS_COUNT; i++)
        strncpy(contact->emails[i], "", MAX_EMAIL_LENGTH - 1);
    for (size_t j = 0; j < MAX_SOCIALS_COUNT; j++)
        strncpy(contact->socialsLink[j], "", SOCIALS_LINK_LENGTH - 1);
    return contact;
}

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
)
{
    if (lastName == NULL || strlen(lastName) < 1
        || firstName == NULL || strlen(firstName) < 1)
        return NULL;
    strncpy(contact->lastName, lastName, LAST_NAME_LENGTH - 1);
    strncpy(contact->firstName, firstName, FIRST_NAME_LENGTH - 1);
    if (patronim) strncpy(contact->patronim, patronim, PATRONIM_LENGTH - 1);
    if (workPlace) strncpy(contact->workInfo.workPlace, workPlace, COMPANY_NAME_LENGTH - 1);
    if (position) strncpy(contact->workInfo.position, position, POSITION_LENGTH - 1);
    if (workPhone) strncpy(contact->numbers.work, workPhone, PHONE_NUMBER_LENGTH - 1);
    if (personalPhone) strncpy(contact->numbers.personal, personalPhone, PHONE_NUMBER_LENGTH - 1);
    if (homePhone) strncpy(contact->numbers.home, homePhone, PHONE_NUMBER_LENGTH - 1);
    if (extraPhone) strncpy(contact->numbers.extra, extraPhone, PHONE_NUMBER_LENGTH - 1);
    if (emails)
    {
        for (size_t i = 0; i < MAX_EMAILS_COUNT; i++)
            strncpy(contact->emails[i], emails[i], MAX_EMAIL_LENGTH - 1);
    }
    if (socials)
    {
        for (size_t j = 0; j < MAX_SOCIALS_COUNT; j++)
            strncpy(contact->socialsLink[j], socials[j], SOCIALS_LINK_LENGTH - 1);
    }
    return contact;
}

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
)
{
    if (contacts == NULL || contactsCount + 1 > MAX_CONTACTS_COUNT)
        return NULL;
    Contact_t* contact = &contacts[contactsCount];

    if (!copyContactInfo(contact,
        lastName, firstName, patronim,
        workPlace, position,
        workPhone, personalPhone, homePhone, extraPhone,
        emails, socials
    ))
        return NULL;

    contactsCount++;
    return contact;
}

int deleteContact(
    Contact_t* contacts,
    size_t index
)
{
    if (!contacts || index >= MAX_CONTACTS_COUNT || index >= contactsCount)
        return 0;
    for (size_t i = index; i < (size_t)contactsCount; i++)
        contacts[i] = contacts[i + 1];
    if (contactsCount == MAX_CONTACTS_COUNT)
        clearContact(&contacts[MAX_CONTACTS_COUNT - 1]);
    contactsCount--;
    return 1;
}

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
)
{
    if (contact == NULL)
        return NULL;

    if (!copyContactInfo(contact,
        lastName, firstName, patronim,
        workPlace, position,
        workPhone, personalPhone, homePhone, extraPhone,
        emails, socials
    ))
        return NULL;

    return contact;
}