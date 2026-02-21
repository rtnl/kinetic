#pragma once

#include <ctime>
#include <string>
#include <chrono>
#include <stdexcept>
#include <sstream>

#include "type.h"
#include "timezone.h"

namespace kinetic {

enum class DateFormat { // inspired from https://github.com/golang/go/blob/master/src/time/format.go
  DateTime,    // "2006-01-02 15:04:05"
  DateOnly,    // "2006-01-02"
  DateTimeDMY, // "02/01/2006 15:04:05"
  DateOnlyDMY, // "02/01/2006"
};

class DateRecord {
private: // Todo: add timezone
  const u64 _year;
  const u64 _month;
  const u64 _day;
  const u64 _hour;
  const u64 _minute;
  const u64 _second;
  const u64 _millisecond;

public:
  DateRecord(
    const u64 year,
    const u64 month,
    const u64 day,
    const u64 hour,
    const u64 minute,
    const u64 second,
    const u64 millisecond
  )
    : _year(year)
    , _month(month)
    , _day(day)
    , _hour(hour)
    , _minute(minute)
    , _second(second)
    , _millisecond(millisecond)
  {}

  u64 get_year() const {
    return _year;
  }

  u64 get_month() const {
    return _month;
  }

  u64 get_day() const {
    return _day;
  }

  u64 get_hour() const {
    return _hour;
  }

  u64 get_minute() const {
    return _minute;
  }

  u64 get_second() const {
    return _second;
  }

  u64 get_millisecond() const {
    return _millisecond;
  }

  DateRecord operator+(const DateRecord & other) const {
    return DateRecord(
      other.get_year()        + _year,
      other.get_month()       + _month,
      other.get_day()         + _day,
      other.get_hour()        + _hour,
      other.get_minute()      + _minute,
      other.get_second()      + _second,
      other.get_millisecond() + _millisecond
    );
  }
};

class Date {
public:
  static u64 get_unix_milli() {
    const auto now = std::chrono::system_clock::now();
    const auto since = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    const auto count = since.count();

    if (count < 0) {
      throw std::runtime_error("time went backwards");
    }

    return static_cast<u64>(since.count());
  }

  static std::string get_dateformat_fmt(const DateFormat f) {
    switch (f) {
      case DateFormat::DateTime:    return "%Y-%m-%d %H:%M:%S";
      case DateFormat::DateOnly:    return "%Y-%m-%d";
      case DateFormat::DateTimeDMY: return "%d/%m/%Y %H:%M:%S";
      case DateFormat::DateOnlyDMY: return "%d/%m/%Y";

      default:
        throw std::runtime_error("unimplemented");
    }
  }

  static void print(std::ostream & writer, u64 ts_milli, const DateFormat fmt = DateFormat::DateTime) { // Todo: use kinetic::Template
    std::time_t t = static_cast<std::time_t>(ts_milli / 1000);
    std::tm tm = {};
    gmtime_r(&t, &tm);

    const auto fmt_s = Date::get_dateformat_fmt(fmt);

    char buf[256];
    std::strftime(buf, sizeof(buf), fmt_s.c_str(), &tm);

    writer << buf;
  }

  static void print(std::ostream & writer, const DateFormat fmt = DateFormat::DateTime) {
    print(writer, get_unix_milli(), fmt);
  }
};

}
