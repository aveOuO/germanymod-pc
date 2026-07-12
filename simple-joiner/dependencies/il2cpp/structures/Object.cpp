#include "Object.hpp"

namespace IL2CPP
{
	Class* Object::GetClass() const
	{
		return (Class*)this->klass;
	}

	void* Object::GetFieldPtr(const std::string& fieldName, int indexOffset) const
	{
		return (void*)((uintptr_t)this + GetClass()->GetField(fieldName, indexOffset)->GetOffset());
	}

	void* Object::GetFieldPtr(size_t index) const
	{
		return (void*)((uintptr_t)this + GetClass()->GetField(index)->GetOffset());
	}
}