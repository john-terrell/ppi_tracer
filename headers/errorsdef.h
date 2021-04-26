#ifndef INCLUDE_ERRORSDEF_H
#define INCLUDE_ERRORSDEF_H

/*
** Standard errors returned by most functions
*/
#define ERROR_OK      0
#define ERROR_NOMEM   1

/* Errors relating to IO. */
#define ERROR_FILE_NOT_IFF            400
#define ERROR_FILE_BAD_TYPE           401
#define ERROR_FILE_CORRUPT_FILE       402
#define ERROR_FILE_NOFILE             403

#define ERROR_FILE_UNKNOWN_SECTION    410

#endif /* INCLUDE_ERRORSDEF_H */
