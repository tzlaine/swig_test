#pragma once

#include "Runtime/UMG/Public/UMG.h"

#include <boost/function.hpp>


#define STYLEABLE_WIDGET_PUBLIC()                                       \
public:                                                                 \
    virtual TSharedRef<SWidget> RebuildWidget () override;              \
                                                                        \
    void set_style (FString const & style_path)

#define STYLEABLE_WIDGET_PRIVATE()                                      \
private:                                                                \
    void apply_style ();                                                \
                                                                        \
    USlateWidgetStyleAsset* style_asset_;                               \
    bool built_

#define STYLEABLE_WIDGET_IMPL_1(name)                                   \
    name::name () :                                                     \
        style_asset_ (nullptr),                                         \
        built_ (false)                                                  \
    {                                                                   \
        auto const & defaults = ui_defaults();

#define STYLEABLE_WIDGET_IMPL_2(name)                                   \
    }                                                                   \
                                                                        \
    TSharedRef<SWidget> name::RebuildWidget ()                          \
    {                                                                   \
        auto retval = Super::RebuildWidget();                           \
        apply_style();                                                  \
        built_ = true;                                                  \
        return retval;                                                  \
    }

#define STYLEABLE_WIDGET_IMPL_SET_STYLE(name)                           \
    void name::set_style (FString const & style_path)                   \
    {                                                                   \
        detail::FObjectFinder<USlateWidgetStyleAsset> style(*style_path); \
        style_asset_ = style.Object;                                    \
                                                                        \
        if (built_)                                                     \
            apply_style();                                              \
    }


namespace detail {

    /** If there is an object class, strips it off. */
    inline void StripObjectClass( FString& PathName, bool bAssertOnBadPath = false )
    {
        int32 NameStartIndex = INDEX_NONE;
        PathName.FindChar( TCHAR('\''), NameStartIndex );
        if( NameStartIndex != INDEX_NONE )
        {
            int32 NameEndIndex = INDEX_NONE;
            PathName.FindLastChar( TCHAR('\''), NameEndIndex );
            if(NameEndIndex > NameStartIndex)
            {
                PathName = PathName.Mid( NameStartIndex+1, NameEndIndex-NameStartIndex-1 );
            }
            else
            {
                UE_CLOG( bAssertOnBadPath, LogUObjectGlobals, Fatal, TEXT("Bad path name: %s, missing \' or an incorrect format"), *PathName );
            }
        }
    }

    inline void FailedToFind(const TCHAR* ObjectToFind)
    {
        FObjectInitializer* CurrentInitializer = FUObjectThreadContext::Get().TopInitializer();
        const FString Message = FString::Printf(TEXT("CDO Constructor (%s): Failed to find %s\n"),
                                                (CurrentInitializer && CurrentInitializer->GetClass()) ? *CurrentInitializer->GetClass()->GetName() : TEXT("Unknown"),
                                                ObjectToFind);
        FPlatformMisc::LowLevelOutputDebugString(*Message);
        UClass::GetDefaultPropertiesFeedbackContext().Log(ELogVerbosity::Error, *Message);
    }

    inline void ValidateObject(UObject *Object, const FString& PathName, const TCHAR* ObjectToFind)
    {
        if (!Object)
        {
            FailedToFind(ObjectToFind);
        }
#if UE_BUILD_DEBUG
        else 
        {
            CheckFoundViaRedirect(Object, PathName, ObjectToFind);
        }
#endif
    }

    inline void CheckFoundViaRedirect(UObject *Object, const FString& PathName, const TCHAR* ObjectToFind)
    {
        UObjectRedirector* Redir = FindObject<UObjectRedirector>(ANY_PACKAGE, *PathName);
        if (Redir && Redir->DestinationObject == Object)
        {
            FString NewString = Object->GetFullName();
            NewString.ReplaceInline(TEXT(" "), TEXT("'"), ESearchCase::CaseSensitive);
            NewString += TEXT("'");

            FObjectInitializer* CurrentInitializer = FUObjectThreadContext::Get().TopInitializer();
            const FString Message = FString::Printf(TEXT("CDO Constructor (%s): Followed redirector (%s), change code to new path (%s)\n"),
                                                    (CurrentInitializer && CurrentInitializer->GetClass()) ? *CurrentInitializer->GetClass()->GetName() : TEXT("Unknown"),
                                                    ObjectToFind, *NewString);

            FPlatformMisc::LowLevelOutputDebugString(*Message);
            UClass::GetDefaultPropertiesFeedbackContext().Log(ELogVerbosity::Warning, *Message);
        }
    }

    template<class T>
    struct FObjectFinder
    {
        T* Object;
        FObjectFinder(const TCHAR* ObjectToFind)
            {
                FString PathName(ObjectToFind);
                StripObjectClass(PathName,true);

                Object = ConstructorHelpersInternal::FindOrLoadObject<T>(PathName);
                ValidateObject( Object, PathName, ObjectToFind );
            }
        bool Succeeded()
            {
                return !!Object;
            }
    };

    template<class T>
    struct FObjectFinderOptional
    {
    private:
        T* Object;
        const TCHAR* ObjectToFind;
    public:
        FObjectFinderOptional(const TCHAR* InObjectToFind)
            : Object(nullptr)
            , ObjectToFind(InObjectToFind)
            {
            }
        T* Get()
            {
                if (!Object && ObjectToFind)
                {
                    FString PathName(ObjectToFind);
                    StripObjectClass(PathName,true);

                    Object = ConstructorHelpersInternal::FindOrLoadObject<T>(PathName);
                    ValidateObject( Object, PathName, ObjectToFind );

                    ObjectToFind = nullptr; // don't try to look again
                }
                return Object;
            }
        bool Succeeded()
            {
                return !!Get();
            }
    };

    template<class T>
    struct FClassFinder
    {
        TSubclassOf<T> Class;
        FClassFinder(const TCHAR* ClassToFind)
            {
                FString PathName(ClassToFind);
                StripObjectClass(PathName, true);
                Class = ConstructorHelpersInternal::FindOrLoadClass(PathName, T::StaticClass());
                ValidateObject(*Class, PathName, *PathName);
            }
        bool Succeeded()
            {
                return !!*Class;
            }
    };

}

struct signal_forwarder
{
    using signal_forward_fn_t = boost::function<void ()>;

    void set_signal_forward_fn(signal_forward_fn_t fn)
    { signal_forward_fn_ = fn; }

protected:
    signal_forward_fn_t signal_forward_fn_;
};
