/**
 * @file error.h
 * @author Gustavo Rehermann (rehermann6046@gmail.com)
 * @brief Error codes.
 * @version added in 0.1
 * @date 2021-03-11
 *
 * @copyright Copyright (c)Gustavo Ramos Rehermann 2021. The MIT License.
 */

// Error codes.
enum error_code_t {
    ERR_INDUSTRY_BAD_INDEX,
    ERR_INDUSTRY_BAD_TYPE,
    ERR_INDUSTRY_BAD_ACCEPT,
};

/**
 * @brief Prints an error.
 * 
 * @param error_code The code of the error to be printed.
 */
void error(enum error_code_t error_code);