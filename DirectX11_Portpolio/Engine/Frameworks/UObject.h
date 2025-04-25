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
 * @tparam Component 컴포넌트의 클래스 타입 전달
 * @param Owner 컴포넌트를 소유 할 액터
 * @param args 생성 할 클래스의 생성자 매개변수
 * @return 생성 된 컴포넌트 반환
 */
template <typename Component, typename... Args>
shared_ptr<Component> UObject::CreateComponent(Actor* Owner, Args&&... args)
{
	static_assert(is_base_of_v<UActorComponent, Component>, 
				  "CreateComponent는 ComponentType은 UActorComponent로 파생된 클래스여야 합니다");

	// 컴포넌트 생성
	auto comp = make_shared<Component>(forward<Args>(args)...);

	// Owner 설정
	comp->SetOwner(Owner);
	
	if constexpr(is_base_of_v<USceneComponent, Component>)
		Owner->AddToOwnedSceneComponents(comp.get());
	else
		Owner->AddToOwnedActorComponents(comp.get());
	
	return comp;
}


/**
 * @tparam ActorType 생성할 Actor의 클래스 전달
 * @param InOuter 생성할 Actor의 Outer
 * @param args Args 생성할 Actor의 생성자 매개변수
 */
template <typename ActorType, typename ... Args>
void UObject::SpawnActor(UObject* InOuter, Args&&... args)
{
	static_assert(is_base_of_v<Actor, ActorType>, 
				  "SpawnActor의 ActorType은 Actor로 파생된 클래스여야 합니다");

	// 컴포넌트 생성
	auto newActor = make_shared<ActorType>(forward<Args>(args)...);

	// Owner 설정
	newActor->Outer = InOuter;
	World::GetLevel()->AddActorToLevel(newActor);
	
}
