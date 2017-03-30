/* open source implementation of a simple thread library.         
 * if you have good ideas are welcome to contact me        
 *                                                         
 * mail2:moecmks.89.5.7@outlook.com  
 * mail1:sosicklove@yahoo.com                                                
 */

#define C4CONTEXT_FULL 1 
#define C4CONTEXT_MAIN 2

#define getcontext_errata_SIP _c4thread_getcontext_errata

#if defined (__x86_32_)
#  define ist_off 0700h 
#elif defined (__x86_WOW64_)
#  define ist_off 01ACh 
#elif defined (__x86_64_)
#  define ist_off 02D0h 
#endif 