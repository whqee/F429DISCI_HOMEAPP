#ifndef __RTC_H
#define __RTC_H

// defined for 32 bit machine
#define bcd2bin(x)	(((x) & 0x0f) + ((x) >> 4) * 10)
#define bin2bcd(x)	((((x) / 10) << 4) + (x) % 10)

#define rtc_read_time(time) rtc->read_time(time)
#define rtc_set_time(time) rtc->set_time(time)

struct rtc_time {
    int sec;
    int min;
    int hour;
    int date; // date of month
    int mon;
    int day;  // day of week
    int year;
};
struct rtc_class_ops {
    int (*read_time)(struct rtc_time *time);
    int (*set_time)(struct rtc_time *time);
};

#define EXPORT(rtc_ops) const struct rtc_class_ops * rtc = rtc_ops
extern const struct rtc_class_ops *rtc;

#endif // !__RTC_H
