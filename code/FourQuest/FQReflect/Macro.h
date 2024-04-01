#pragma once


#define FQ_CAT_IMPL(a, b) a##b
#define FQ_CAT(a, b) FQ_CAT_IMPL(a, b)

static void fq_auto_register_reflection_function_();

#define FQ_REGISTRATION_FRIEND friend void ::fq_auto_register_reflection_function_();

#define FQ_REGISTRATION                                                           \
static void fq_auto_register_reflection_function_();                              \
namespace                                                                         \
{                                                                                 \
    struct fq__auto__register__                                                   \
    {                                                                             \
        fq__auto__register__()                                                    \
        {                                                                         \
            fq_auto_register_reflection_function_();                              \
        }                                                                         \
    };                                                                            \
}                                                                                 \
static const fq__auto__register__ FQ_CAT(auto_register__, __LINE__);            \
static void fq_auto_register_reflection_function_()
