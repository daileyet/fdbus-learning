#ifndef __CFDBIFPERSION_H__ 
#define __CFDBIFPERSION_H__

#include <common_base/fdbus.h>
#include <iostream>

enum EMessageId
{
    REQ_METADATA,
    REQ_RAWDATA,
    REQ_CREATE_MEDIAPLAYER,
    NTF_ELAPSE_TIME,
    NTF_MEDIAPLAYER_CREATED,
    NTF_MANUAL_UPDATE
};
// define a message with class being "CCar". It should be subclass of IFdbParcelable
class CCar : public IFdbParcelable
{
public:
   /*
    * callback that should be implemented to encode data field into wire format. "serializer"
    * is the serializion engine of FDBus supporting most scalar types, string type,
    * IFdbParcelable type (subclass of IFdbParcelable) and array of the types.
    */
   void serialize(CFdbSimpleSerializer &serializer) const
   {
       serializer << mBrand << mModel << mPrice;
   }
   /*
    * callback that should be implemented to decode wire format into data field. "deserializer"
    * is the serializion engine of FDBus supporting deserialization of most scalar types,
    * string type and IFdbParcelable type (subclass of IFdbParcelable). Note that the order of
    * data field connected by ">>" should be the same as that specified in serializer() above
    */
   void deserialize(CFdbSimpleDeserializer &deserializer)
   {
       deserializer >> mBrand >> mModel >> mPrice;
   }
   // data field of the message
   std::string mBrand;
   std::string mModel;
   int32_t mPrice;
protected:
   /*
    * callback turning the message into human-readable string. It
    * is called when log server is launched to capture FDBus message
    */
   void toString(std::ostringstream &stream) const
   {
       stream << "mBrand:" << mBrand
              << ", mModel:" << mModel
              << ", mPrice:" << mPrice;
   }
};

// define another message with class being "CPerson". It should be subclass of IFdbParcelable
class CPerson : public IFdbParcelable
{
public:
   // refer to CCar
   void serialize(CFdbSimpleSerializer &serializer) const
   {
       serializer << mName << mAge << mCars << mAddress << mSalary;
   }
   // refer to CCar
   void deserialize(CFdbSimpleDeserializer &deserializer)
   {
       deserializer >> mName >> mAge >> mCars >> mAddress >> mSalary;
   }
   // data field of message
   std::string mName;
   uint8_t mAge;
   int32_t mSalary;
   std::string mAddress;

   /*
    * This is a special case: it define an array of CCar objects. CFdbParcelableArray<>
    * can be bound with all supported types to define an array of data/message. Since
    * CFdbParcelableArray is also subclass of IFdbParcelable, it can be accepted by
    * serializer/deserializer (see above). Number of elements in the array can be vary.
    */
   CFdbParcelableArray<CCar> mCars;
   /*
    * callback turning the message into human-readable string. It
    * is called when log server is launched to capture FDBus message
    */
   void toString(std::ostringstream &stream) const
   {
       // Note that mCars can not be fed to stream directly!!!
       stream << "mName:" << mName
              << ", mAge:" << (unsigned)mAge
              << ", mSalary:" << mSalary
              << ", mCars:"; mCars.format(stream)
              << ", mAddress:" << mAddress;
   }
};

#endif