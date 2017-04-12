/*
 * MIT License
 * Copyright (c) 2017 Kevin Kirchner
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * \file        libiban.cpp
 * \brief       Main source file for \p libiban
 * \author      Kevin Kirchner
 * \date        2017
 * \copyright   MIT LICENSE
 *
 * This source file is the main source file for \p libiban and implements the
 * classes and functions of the library.
 */

#include <iostream>
#include "libiban.h"
#include "utils.h"

namespace IBAN {

    /**
     * Constructor of \p IBANParseException.
     *
     * @param iban The IBAN string causing the exception
     */
    IBANParseException::IBANParseException(const std::string &iban) noexcept :
            m_iban(iban) {
        m_message = "Cannot parse IBAN " + m_iban;
    }

    /**
     * Destructor of \p IBANParseException
     */
    IBANParseException::~IBANParseException() {}

    /**
     * Overrides the function \p what() for this exception and returns a
     * string describing the error.
     *
     * @return A string describing the error
     */
    const char *IBANParseException::what() const noexcept {
        return m_message.c_str();
    }

    // initialize country code map
    const map_t IBAN::m_countryCodes = {
        {"AL", 28}, {"AD", 24}, {"AT", 20}, {"AZ", 28}, {"BE", 16},
        {"BH", 22}, {"BA", 20}, {"BR", 29}, {"BG", 22}, {"CR", 22},
        {"HR", 21}, {"CY", 28}, {"CZ", 24}, {"DK", 18}, {"DO", 28},
        {"EE", 20}, {"FO", 18}, {"FI", 18}, {"FR", 27}, {"GE", 22},
        {"DE", 22}, {"GI", 23}, {"GR", 27}, {"GL", 18}, {"GT", 28},
        {"HU", 28}, {"IS", 26}, {"IE", 22}, {"IL", 23}, {"IT", 27},
        {"KZ", 20}, {"KW", 30}, {"LV", 21}, {"LB", 28}, {"LI", 21},
        {"LT", 20}, {"LU", 20}, {"MK", 19}, {"MT", 31}, {"MR", 27},
        {"MU", 30}, {"MC", 27}, {"MD", 24}, {"ME", 22}, {"NL", 18},
        {"NO", 15}, {"PK", 24}, {"PS", 29}, {"PL", 28}, {"PT", 25},
        {"RO", 24}, {"SM", 27}, {"SA", 24}, {"RS", 22}, {"SK", 24},
        {"SI", 19}, {"ES", 24}, {"SE", 24}, {"CH", 21}, {"TN", 24},
        {"TR", 26}, {"AE", 23}, {"GB", 22}, {"VG", 24}, {"BJ", 28},
        {"BF", 28}, {"BI", 16}, {"CM", 27}, {"CV", 25}, {"TL", 23},
        {"IR", 26}, {"CI", 28}, {"JO", 30}, {"SA", 24}, {"MG", 27},
        {"ML", 28}, {"MZ", 25}, {"QA", 29}, {"XK", 20}, {"SN", 28},
        {"LC", 32}, {"ST", 25}, {"UA", 29}, {"SC", 31}, {"IQ", 23},
        {"BY", 28}, {"SV", 28}, {"AO", 25}, {"CF", 27}, {"CG", 27},
        {"EG", 27}, {"DJ", 27}, {"DZ", 24}, {"GA", 27}, {"GQ", 27},
        {"GW", 25}, {"MA", 28}, {"NE", 28}, {"TD", 27}, {"TG", 28},
        {"KM", 27}, {"HN", 28}, {"NI", 32},
    };

    /**
     * Tries to create a new instance of \p IBAN from a string parameter. If
     * parsing fails, the methods throws an \p IBANParseException. This happens
     * if the string is too short to be an IBAN number or if it contains invalid
     * characters.
     *
     * Note that this does not guarantee the validity of the IBAN number. Call
     * \p validate() to test for validity.
     *
     * @param string The string to create an IBAN from
     * @return A new instance of \p IBAN
     */
    IBAN IBAN::createFromString(const std::string &string) {
        std::string s;
        std::string countryCode{}, accID{};
        size_t checkSum{};

        s = const_cast<std::string&>(string);
        trim(s);
        std::transform(s.begin(), s.end(), s.begin(), ::toupper);
        // too long or too short
        if (s.length() > 34 || s.length() < 5) {
            throw IBANParseException(string);
        }

        // first to chars are country code
        countryCode = s.substr(0, 2);
        if (!isalpha(countryCode[0]) || !isalpha(countryCode[1])) {
            throw IBANParseException(string);
        }
        // then two chars for the check sum
        try {
            checkSum = static_cast<size_t>(std::stoi(s.substr(2, 2)));
        } catch (const std::exception) {
            throw IBANParseException(string);
        }
        // rest is account ID
        accID = s.substr(4);
        for (auto ch : accID) {
            if (!isalnum(ch)) {
                throw IBANParseException(string);
            }
        }

        return IBAN(countryCode, accID, checkSum);
    }

    /**
     * Return the account identifier part of the IBAN number.
     *
     * @return The account identifier of the IBAN
     */
    std::string IBAN::getAccountIdentifier() const {
        return m_accountIdentifier;
    }

    /**
     * Return the checksum part of the IBAN number.
     *
     * @return The checksum of the IBAN
     */
    size_t IBAN::getChecksum() const {
        return m_checkSum;
    }

    /**
     * Return the country code part of the IBAN number.
     *
     * @return The country code of the IBAN
     */
    std::string IBAN::getCountryCode() const {
        return m_countryCode;
    }

    /**
     * Returns the machine friendly formatting of the IBAN number which does
     * not contain any spaces.
     *
     * @return Machine friendly representation of the IBAN
     */
    std::string IBAN::getMachineForm() {
        return m_countryCode + std::to_string(m_checkSum) + m_accountIdentifier;
    }

    /**
     * Returns the human readable formatting of the IBAN number which formats
     * the IBAN number into blocks of 4 characters.
     *
     * @return Human readable representation of the IBAN
     */
    std::string IBAN::getHumanReadable() {
        std::stringstream result("");
        result << m_countryCode << m_checkSum;
        for (size_t i = 0; i < m_accountIdentifier.length(); i += 5) {
            m_accountIdentifier.insert(i, 1, ' ');
        }
        result << m_accountIdentifier;
        return result.str();
    }

    /**
     * Validates the underlying object according to the IBAN format
     * specification and returns a boolean value indicating validation status.
     *
     * @return \p true if IBAN is valid, \p false otherwise
     */
    bool IBAN::validate() const {
        // invalid country code
        if (m_countryCodes.find(m_countryCode) == m_countryCodes.end()) {
            return false;
        }
        size_t expectedLength = m_countryCodes.find(m_countryCode)->second;

        // pad checksum
        std::string checksum = std::to_string(m_checkSum);
        while (checksum.length() < 2) {
            checksum = "0" + checksum;
        }

        // concat string and check length
        std::string checkString = m_accountIdentifier + m_countryCode + checksum;
        if (checkString.length() != expectedLength) {
            return false;
        }

        std::string numeric = "";

        // string substitution
        for (auto ch : checkString) {
            if (std::isdigit(ch)) {
                numeric += ch;        // convert numeric char to integer
            } else if (std::isalpha(ch)) {
                // get position in latin alphabet and add 9
                numeric += std::to_string((31 & ch) + 9);
            } else {
                return false;
            }
        }

        size_t seg = 0;
        size_t step = 9;
        std::string prepended {};
        long number = 0;

        // the numeric string does not fit into any long integer data type,
        // so we have to check the string stepwise; first a single 9 char
        // step, then 7 char steps until we reach the end
        while (seg  < numeric.length( ) - step ) {
            number = std::stol(prepended + numeric.substr(seg , step));
            int remainder = static_cast<int>(number % 97);
            prepended = std::to_string(remainder);

            if (remainder < 10) {
                prepended = "0" + prepended;
            }
            seg += step;
            step = 7;
        }
        number = std::stol(prepended + numeric.substr(seg)) ;
        return (number % 97 == 1);
    }
}
