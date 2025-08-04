#include "contacts.h"

ssize_t readContacts(const char* filename)
{
    int file = open(filename,
        O_RDONLY | O_CREAT,
        S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (file == -1)
    {
        perror("readContact");
        return -1;
    }

    ssize_t count = 1;

    Contact_t buffer;
    ssize_t readBytes;
    readBytes = read(file, &buffer, sizeof(buffer));
    while (count <= MAX_CONTACTS_COUNT)
    {
        if (readBytes == -1)
        {
            perror("readContacts");
            return -1;
        }
        if (!readBytes)
            break;
        contacts[count - 1] = buffer;
        count++;
        readBytes = read(file, &buffer, sizeof(buffer));
    }

    contactsCount = count - 1;
    if (close(file) == -1)
        perror("readContacts");
    return contactsCount;
}

ssize_t writeContacts(const char* filename)
{
    int file = open(filename,
        O_WRONLY | O_CREAT | O_TRUNC,
        S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (file == -1)
    {
        perror("writeContact");
        return -1;
    }

    if (!contactsCount)
    {
        close(file);
        return 0;
    }

    size_t count = 0;

    Contact_t buffer;
    ssize_t writeBytes = 0;
    while (count < contactsCount)
    {
        if (writeBytes == -1)
        {
            perror("writeContacts");
            close(file);
            return -1;
        }
        buffer = contacts[count];
        count++;
        writeBytes = write(file, &buffer, sizeof(buffer));
    }
    if (close(file) == -1)
        perror("readContacts");
    return count;
}