#include "HeaderCollection.h"
#include "RHIResources.h"

FRHIResource::FRHIResource(ERHIResourceType InResourceType)
    :ResourceType(InResourceType)
{
}

FRHIResource::~FRHIResource()
{
}
