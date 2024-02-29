#include "../../inc/MySQLDriver.hpp"

/**
 * public
*/

MySQLDriver& MySQLDriver::getInstance() {
    static MySQLDriver own;
    return own;
}

sql::Driver* MySQLDriver::getDriver() const {   // override
    return driver;
}

/**
 * private
*/

MySQLDriver::MySQLDriver() {
    if(!driver) {
        driver = get_driver_instance();
    }
}

