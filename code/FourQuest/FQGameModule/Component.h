#pragma once

namespace fq::game_module
{
	class GameObject;

	/// <summary>
	/// GameOnject�� �ͼӵǾ� �������� 
	/// ���� ������ ����
	/// </summary>
	class Component
	{
	public:
		Component();
		Component(const Component& other) = default;
		Component& operator =(const Component& other) = default;
		Component(Component&& other) = default;

		virtual ~Component();

		/// <summary>
		/// GameObject�� �����տ��� �ν��Ͻ�ȭ �Ŀ� ȣ��
		/// </summary>
		virtual void OnAwake() {};

		/// <summary>
		/// Scene ���۽� ȣ��
		/// </summary>
		virtual void OnStart() {};
		
		/// <summary>
		/// ������ ���������� ȣ��
		/// </summary>
		/// <param name="dt">FixedDeltaTime</param>
		virtual void OnFixedUpdate(float dt) {};
		
		/// <summary>
		/// �� ������ ȣ��
		/// ������Ʈ�� �⺻���� ������ ����
		/// </summary>
		/// <param name="dt">DeltaTime</param>
		virtual void OnUpdate(float dt) {};
		
		/// <summary>
		/// Scene�� GameObject�� Update ���Ŀ� ȣ��
		/// </summary>
		/// <param name="dt">DeltaTime</param>
		virtual void OnLateUpdate(float dt) {};

		/// <summary>
		/// GameObject �ı��� ȣ��
		/// </summary>
		virtual void OnDestroy() {};

		/// <summary>
		/// ������Ʈ�� �����ϴ� ���ӿ�����Ʈ ��ȯ
		/// </summary>
		/// <returns>���� ������Ʈ</returns>
		GameObject* GetGameObject()const { return mGameObject; }

	private:
		GameObject* mGameObject;
	};


}