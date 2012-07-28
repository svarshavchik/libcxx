#define EPOCHEND 9999

#define LDOM(m,y) ( (m) == 2 ? (leapyear(y) ? 29:28):			\
		    (m) == 9 || (m) == 4 || (m) == 6 || (m) == 11 ? 30:31)

// Number of days in one leap year and three non leap years

#define NDY4 (366 + 365 * 3)

// Number of days in a century that begins with a leap year

#define NDCL (NDY4 * 25)

// Number of days in a century that does not begin with a leap year

#define NDCN (NDCL - 1)

// Number of days in four consecutive centuries

#define NDC4 (NDCL + NDCN * 3)
