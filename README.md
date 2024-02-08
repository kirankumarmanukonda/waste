# waste

https://teams.microsoft.com/l/meetup-join/19%3ameeting_YmMxMzFhODAtMGYzZS00MWFmLWI1YWYtNGIwYjdmMzc3ODA4%40thread.v2/0?context=%7b%22Tid%22%3a%2298e9ba89-e1a1-4e38-9007-8bdabc25de1d%22%2c%22Oid%22%3a%22d86d68c2-f0c5-47f0-aa0a-3ec33675547f%22%7d


clock_t foc_pressd_s, foc_pressd_e, main_pressd_s, main_pressd_e;
double button_foc_time, button_main_time;
void x()
{
    foc_pressd_e = clock();
    button_foc_time = ((double)(foc_pressd_e - foc_pressd_s)) / CLOCKS_PER_SEC * 1000;
    if(button_foc_time > 250 && 1)//read button also
    {
        foc_pressd_s = clock();
        //event

    }
}
