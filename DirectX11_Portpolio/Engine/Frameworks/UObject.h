#pragma once



class World;
class ULevel;
class Actor;
class UActorComponent;
class USceneComponent;

class UObject
{
public:
	UObject();
	virtual ~UObject();

protected:
	template<typename Component, typename... Args>
	shared_ptr<Component> CreateComponent(Actor* Owner, Args&&... args);
	
	template<typename ActorType, typename... Args>
	void SpawnActor(UObject* InOuter, Args&&... args);
	
public:
	atomic<UINT32> ObjectID;
};



/**
 * @tparam Component ������Ʈ�� Ŭ���� Ÿ�� ����
 * @param Owner ������Ʈ�� ���� �� ����
 * @param args ���� �� Ŭ������ ������ �Ű�����
 * @return ���� �� ������Ʈ ��ȯ
 */
template <typename Component, typename... Args>
shared_ptr<Component> UObject::CreateComponent(Actor* Owner, Args&&... args)
{
	static_assert(is_base_of_v<UActorComponent, Component>, 
				  "CreateComponent�� ComponentType�� UActorComponent�� �Ļ��� Ŭ�������� �մϴ�");

	// ������Ʈ ����
	auto comp = make_shared<Component>(forward<Args>(args)...);

	// Owner ����
	comp->SetOwner(Owner);
	
	if constexpr(is_base_of_v<USceneComponent, Component>)
		Owner->AddToOwnedSceneComponents(comp.get());
	else
		Owner->AddToOwnedActorComponents(comp.get());
	
	return comp;
}


/**
 * @tparam ActorType ������ Actor�� Ŭ���� ����
 * @param InOuter ������ Actor�� Outer
 * @param args Args ������ Actor�� ������ �Ű�����
 */
template <typename ActorType, typename ... Args>
void UObject::SpawnActor(UObject* InOuter, Args&&... args)
{
	static_assert(is_base_of_v<Actor, ActorType>, 
				  "SpawnActor�� ActorType�� Actor�� �Ļ��� Ŭ�������� �մϴ�");

	// ������Ʈ ����
	auto newActor = make_shared<ActorType>(forward<Args>(args)...);

	// Owner ����
	newActor->Outer = InOuter;
	World::GetLevel()->AddActorToLevel(newActor);
	
}
