#pragma once

namespace golem
{
	template <typename T>
	struct TreeNode
	{
		// Members ********************************************************************************
	public:
		T* parent = nullptr;
		std::vector<T*> children;

	protected:
		TreeNode() = default;
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