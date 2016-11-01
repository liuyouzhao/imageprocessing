#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include "proc.h"
#include "proc_utils.h"

#if USE_GROUP
int hessian_value(u32* integral, double* values, int w, int h, int level, int group);
int is_hessian_max(double* vu, double* vm, double* vd, int x, int y, int w, int h, int l, int g);
#else
int hessian_value(u32* integral, double* values, int w, int h, int level);
int is_hessian_max(double* vu, double* vm, double* vd, int x, int y, int w, int h, int l);
#endif
int hessian_get_direction(int x, int y, double s, int l, u32* integral, int w, int h);
int hessian_descript(u8* image, int w, int h, int x, int y, int dir, int* out, double s);

extern HaarlikeProc g_haarlike;
extern int s_level[6];

/**
  size-level parimit
*/

SurfObject::SurfObject()
{
    m_integral = NULL;
    m_orignal_image = NULL;
    m_width = 0;
    m_height = 0;
}

SurfObject::~SurfObject()
{
    free(m_integral);
    free(m_orignal_image);
}

void SurfObject::updateImage(u8* image, int w, int h)
{
    if(!m_orignal_image)
    {
        m_orignal_image = (u8*) malloc(w * h);
    }
    memcpy(m_orignal_image, image, w * h);
    m_width = w;
    m_height = h;
}

void SurfObject::updateCurrentIntegral()
{
    if(!m_integral)
        m_integral = (u32*) malloc(sizeof(u32) * m_width * m_height);
    g_haarlike.haarlike_integral(m_orignal_image, m_integral, m_width, m_height);
}

void SurfObject::updateAllParimitsFeatures()
{
    int w = m_width;
    int h = m_height;

    if(m_parimit.size() == 0)
    {
#if USE_GROUP
        for(int i = 0; i < PARIMITS_LAYERS_NUMBER * 3; i ++)
        {
            struct __hessian_value* hv = (struct __hessian_value*)malloc(sizeof(struct __hessian_value));
            hv->values = (double*) malloc(w * h * sizeof(double));
            hv->level = i % PARIMITS_LAYERS_NUMBER;
            hv->group = i / PARIMITS_LAYERS_NUMBER;
            m_parimit.push_back(hv);
        }
#else
        for(int i = 0; i < PARIMITS_LAYERS_NUMBER; i ++)
        {
            struct __hessian_value* hv = (struct __hessian_value*)malloc(sizeof(struct __hessian_value));
            hv->values = (double*) malloc(w * h * sizeof(double));
            hv->level = i;
            hv->group = 0;
            m_parimit.push_back(hv);
        }
#endif
    }

    u8 image[w * h];
    std::vector<struct __hessian_value*>::iterator it;
    for(it = m_parimit.begin(); it != m_parimit.end(); it ++)
    {
        struct __hessian_value* v = *it;
#if USE_GROUP
        hessian_value(m_integral, v->values, m_width, m_height, v->level, v->group);
#else
        hessian_value(m_integral, v->values, m_width, m_height, v->level);
#endif

#if 0
        for(int i = 0; i < w * h; i ++)
        {
            double vv = v->values[i];

            image[i] = (u8)vv;
        }
        show_image(image, w, h);
#endif
    }
}

void SurfObject::updateCalculateExtremums()
{
    int w = m_width;
    int h = m_height;

    std::vector<struct __hessian_fv*>::iterator it_del;
    it_del = m_feature_values.begin();
    for( ; it_del != m_feature_values.end(); it_del ++ )
    {
        struct __hessian_fv* fv = *it_del;
        free(fv);
    }
    m_feature_values.clear();


    std::vector<struct __hessian_value*>::iterator it;
    it = m_parimit.begin() + 1;
    for(int l = 1; l < PARIMITS_LAYERS_NUMBER - 1; l ++, it ++)
    {
        struct __hessian_value* vu = *(it - 1);
        struct __hessian_value* vd = *(it + 1);
        struct __hessian_value* vm = *it;
        struct __hessian_fv* fv_new = 0;
        int index = 0;
        int maxx = -99;
        for(int y = 1; y < h - 1; y ++) {
            for(int x = 1; x < w - 1; x ++)
            {
                index = y * w + x;
                if(vm->values[index] < FEATURE_THREHOLD)
                {
                    continue;
                }
#if USE_GROUP
                int is_max = is_hessian_max(vu->values, vm->values, vd->values, x, y, w, h, vm->level, vm->group);
#else
                int is_max = is_hessian_max(vu->values, vm->values, vd->values, x, y, w, h, vm->level);
#endif
                int v = vm->values[index];
                if(is_max)
                {
                    fv_new = (struct __hessian_fv*)malloc(sizeof(struct __hessian_fv));
                    fv_new->x = x;
                    fv_new->y = y;
                    fv_new->hfv = v;
                    fv_new->level = l;
                    fv_new->group = vm->group;
                    int size = s_level[fv_new->level];
                    double s = (double)size / 9.0f * 1.2f;
                    fv_new->s = s;
                    m_feature_values.push_back(fv_new);

                }
            }
        }
        //printf("%d \n", maxx);
    }
}


void SurfObject::updateCalculateDirections()
{
    std::vector<struct __hessian_fv*>::iterator it;
    it = m_feature_values.begin();
    for( ; it != m_feature_values.end(); it ++ )
    {
        struct __hessian_fv* fv = *it;
        /// TODO: calculate direction

        int dir = hessian_get_direction(fv->x, fv->y, fv->s, fv->level, m_integral, m_width, m_height);
        fv->dir = dir;
        //printf("dir: %d \n", dir);
        //printf("%d ", dir);
    }
}

void SurfObject::updateFeatureDescription()
{
    /// TODO: calculate haar waves hessian description
    std::vector<struct __hessian_fv*>::iterator it;
    it = m_feature_values.begin();
    for( ; it != m_feature_values.end(); it ++ )
    {
        struct __hessian_fv* fv = *it;
        /// TODO: calculate direction
        printf("%d %d %d %d \n", fv->x, fv->y, fv->dir, fv->level);
        int size = s_level[fv->level];
        double s = (double)size / 9.0f * 1.2f;
        hessian_descript(m_orignal_image, m_width, m_height, fv->x, fv->y, fv->dir, fv->fvs, s);
#if 0
        for(int i = 0; i < 64; i ++)
        {
            printf("%d ", fv->fvs[i]);
        }
        printf("\n\n");
#endif
    }

}
