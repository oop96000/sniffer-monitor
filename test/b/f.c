
struct caltime {
time_t nowsec;
time_t nowmin;
time_t nowh;
time_t nowd;
time_t nowm;
time_t nowy;

};

struct caltime sec2cal(time_t cur_second)
{
struct caltime cur_time;
int runnian;
int year;
/* Fill the data into struct cur_time */
cur_time.nowsec = (cur_second % 60);
cur_time.nowmin = (cur_second - cur_time.nowsec)/60%60;
cur_time.nowh = (cur_second - cur_time.nowmin * 60 - cur_time.nowsec -
 8 * 3600 )/3600%24 ;
cur_time.nowd = (cur_second - cur_time.nowh * 3600 - cur_time.nowmin *
 60 - cur_time.nowsec)/86400;

cur_time.nowy = 1970;
while (cur_time.nowd > 365) {
year = cur_time.nowy + 1;
if ((year%4 == 0 && year%100 != 0) || (year%400 == 0))
runnian = 1;
else runnian = 0;
if (runnian==1) {
cur_time.nowd = cur_time.nowd - 366;
cur_time.nowy += 1;
}
else {
cur_time.nowd = cur_time.nowd - 365;
cur_time.nowy += 1;
}
}
if ((cur_time.nowy % 4 == 0 && cur_time.nowy % 100 != 0) ||
(cur_time.nowy %400 == 0)) {/* cur_time.nowy is a leap year */
if (cur_time.nowd <= 31)
cur_time.nowm = 1;
if ( cur_time.nowd >31 && cur_time.nowd <= 60) {
cur_time.nowm = 2;
cur_time.nowd = cur_time.nowd - 31;
}
if ( cur_time.nowd > 60 && cur_time.nowd <= 91) {
cur_time.nowm = 3;
cur_time.nowd = cur_time.nowd - 60;
}
if ( cur_time.nowd > 91 && cur_time.nowd < 121) {
cur_time.nowm = 4;
cur_time.nowd = cur_time.nowd - 91;
}
if ( cur_time.nowd > 121 && cur_time.nowd <= 152) {
cur_time.nowm = 5;
cur_time.nowd = cur_time.nowd - 121;
}
if ( cur_time.nowd > 152 && cur_time.nowd <= 182) {
cur_time.nowm = 6;
cur_time.nowd = cur_time.nowd - 152;
}
if ( cur_time.nowd >182 && cur_time.nowd<= 213) {
cur_time.nowm = 7;
cur_time.nowd = cur_time.nowd - 182;
}
if ( cur_time.nowd > 213 && cur_time.nowd <= 244) {
cur_time.nowm = 8;
cur_time.nowd = cur_time.nowd - 213;
}
if ( cur_time.nowd > 244 && cur_time.nowd <= 274) {
cur_time.nowm = 9;
cur_time.nowd = cur_time.nowd - 244;
}
if ( cur_time.nowd > 274 && cur_time.nowd <= 305) {
cur_time.nowm = 10;
cur_time.nowd = cur_time.nowd - 274;
}
if ( cur_time.nowd > 305 && cur_time.nowd <= 335) {
cur_time.nowm = 11;
cur_time.nowd = cur_time.nowd - 305;
}
if ( cur_time.nowd > 335) {
cur_time.nowm = 12;
cur_time.nowd = cur_time.nowd - 335;
}
}
else {
if (cur_time.nowd <= 31)
cur_time.nowm = 1;
if ( cur_time.nowd > 31 && cur_time.nowd <= 59) {
cur_time.nowm = 2;
cur_time.nowd = cur_time.nowd - 31;
}
if ( cur_time.nowd > 59 && cur_time.nowd <= 90) {
cur_time.nowm = 3;
cur_time.nowd = cur_time.nowd - 59;
}
if ( cur_time.nowd > 90 && cur_time.nowd <= 120) {
cur_time.nowm = 4;
cur_time.nowd = cur_time.nowd - 90;
}
if ( cur_time.nowd > 120 && cur_time.nowd <= 151) {
cur_time.nowm = 5;
cur_time.nowd = cur_time.nowd - 120;
}
if ( cur_time.nowd > 151 && cur_time.nowd <= 181) {
cur_time.nowm = 6;
cur_time.nowd = cur_time.nowd - 151;
}
if ( cur_time.nowd > 181 && cur_time.nowd <= 211) {
cur_time.nowm = 7;
cur_time.nowd = cur_time.nowd - 181;
}
if ( cur_time.nowd > 211 && cur_time.nowd <= 243) {
cur_time.nowm = 8;
cur_time.nowd = cur_time.nowd - 212;
}
if ( cur_time.nowd > 243 && cur_time.nowd <= 273) {
cur_time.nowm = 9;
cur_time.nowd = cur_time.nowd - 243;
}
if ( cur_time.nowd > 273 && cur_time.nowd <= 304) {
cur_time.nowm = 10;
cur_time.nowd = cur_time.nowd - 273;
}
if ( cur_time.nowd > 304 && cur_time.nowd <= 33334) {
cur_time.nowm = 11;
cur_time.nowd = cur_time.nowd - 304;
}
if ( cur_time.nowd > 334) {
cur_time.nowm = 12;
cur_time.nowd = cur_time.nowd - 334;
}
}
cur_time.nowd += 1;
printk("%d-%d-%d
   %d:%d:%d\n",cur_time.nowy,cur_time.nowm,cur_time.nowd,
   cur_time.nowh,cur_time.nowmin,cur_time.nowsec);
return cur_time;
}
