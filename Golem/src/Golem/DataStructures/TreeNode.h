#pragma once

namespace golem
{
	template <typename T>
	struct TreeNode
	{
		// Members ********************************************************************************
	public:
		TreeNode() = default;

		T* Parent() {return parent;}

		T* Child(int index) {return children[index];}
		std::vector<T*> Children() {return children;}

		void SetParent(T* p)
		{
			parent = p;
			p->children.push_back(static_cast<T*>(this));
		}

		void SetChild(T* c)
		{
			children.push_back(c);
			c->parent = static_cast<T*>(this);
		}

	protected:
		T* parent = nullptr;
		std::vector<T*> children;

	};

	template <typename T, typename...Args>
	void TreeLevelOrderTraversal(TreeNode<T>* root, std::function<void(TreeNode<T>*, Args...)> func, Args... args)
	{
		GOL_CORE_ASSERT(root != nullptr, "root node is null");
		GOL_CORE_ASSERT(func != nullptr, "func is null");

		std::queue<TreeNode<T>*> q;
		q.push(root);

		while (!q.empty())
		{
			int currentLayer = q.size();

			// If this node has children
			while (currentLayer > 0)
			{
				TreeNode<T>* node = q.front();
				q.pop();

				// run function
				func(node, args...);

				// Enqueue all children of the dequeued node
				for (int i = 0; i < node->children.size(); i++)
					q.push(node->children[i]);
				currentLayer--;
			}
		}
	}
}