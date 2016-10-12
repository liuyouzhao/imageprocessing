#if 0
/// Need check conflict ?
        int conflict = 0;

        if(g_strocls->nwk > 0) {
            std::vector<struct __clssf*>::iterator it;
            for( it = g_strocls->vwkcs.begin(); it != g_strocls->vwkcs.end(); it ++ ) {

                struct __clssf* p = *it;
                if(p == wkcls) {
                    conflict ++;
                }
            }
        }


        if(!conflict || g_strocls->nwk == 0) {
            g_strocls->vwkcs.push_back(wkcls);
            g_strocls->nwk ++;
        }
#endif
