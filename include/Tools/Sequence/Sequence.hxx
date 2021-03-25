#include <set>

#include "Module/Subsequence/Subsequence.hpp"
#include "Tools/Sequence/Sequence.hpp"

namespace aff3ct
{
namespace tools
{
size_t Sequence
::get_n_threads() const
{
	return this->n_threads;
}

const std::vector<std::vector<module::Task*>>& Sequence
::get_firsts_tasks() const
{
	return this->firsts_tasks;
}

const std::vector<std::vector<module::Task*>>& Sequence
::get_lasts_tasks() const
{
	return this->lasts_tasks;
}

template <class C>
std::vector<C*> Sequence
::get_modules(const bool subsequence_modules) const
{
	std::vector<C*> ret;
	for (auto &mm : this->all_modules)
		for (auto &m : mm)
		{
			if (subsequence_modules)
			{
				auto c = dynamic_cast<module::Subsequence*>(m);
				if (c != nullptr)
				{
					auto subret = c->get_sequence().get_modules<C>(subsequence_modules);
					ret.insert(ret.end(), subret.begin(), subret.end());
				}
			}

			auto c = dynamic_cast<C*>(m);
			if (c != nullptr)
				ret.push_back(c);
		}

	return ret;
}

template <class C>
std::vector<C*> Sequence
::get_cloned_modules(const C &module_ref) const
{
	bool found = false;
	size_t mid = 0;
	while (mid < this->all_modules[0].size() && !found)
		if (dynamic_cast<C*>(this->all_modules[0][mid]) == &module_ref)
			found = true;
		else
			mid++;

	if (!found)
	{
		std::stringstream message;
		message << "'module_ref' can't be found in the sequence.";
		throw tools::runtime_error(__FILE__, __LINE__, __func__, message.str());
	}

	std::vector<C*> cloned_modules(this->all_modules.size());
	for (size_t tid = 0; tid < this->all_modules.size(); tid++)
	{
		auto c = dynamic_cast<C*>(this->all_modules[tid][mid]);
		if (c == nullptr)
		{
			std::stringstream message;
			message << "'c' can't be 'nullptr', this should never happen.";
			throw tools::runtime_error(__FILE__, __LINE__, __func__, message.str());
		}
		cloned_modules[tid] = c;
	}
	return cloned_modules;
}

template <class SS>
inline void Sequence
::_init(Generic_node<SS> *root)
{
	std::stringstream message;
	message << "This should never happen.";
	throw tools::runtime_error(__FILE__, __LINE__, __func__, message.str());
}

template <>
inline void Sequence
::_init(Generic_node<tools::Sub_sequence_const> *root)
{
	this->duplicate<tools::Sub_sequence_const, const module::Module>(root);
	std::vector<Generic_node<Sub_sequence_const>*> already_deleted_nodes;
	this->delete_tree(root, already_deleted_nodes);
}

template <>
inline void Sequence
::_init(Generic_node<tools::Sub_sequence> *root)
{
	this->sequences[0] = root;

	std::set<module::Module*> modules_set;
	std::function<void(const Generic_node<tools::Sub_sequence>*,
	                   std::vector<const Generic_node<tools::Sub_sequence>*>&)> collect_modules_list;
	collect_modules_list = [&](const Generic_node<tools::Sub_sequence> *node,
	                           std::vector<const Generic_node<tools::Sub_sequence>*> &already_parsed_nodes)
	{
		if (node != nullptr &&
		    std::find(already_parsed_nodes.begin(),
		              already_parsed_nodes.end(),
		              node) == already_parsed_nodes.end())
		{
			already_parsed_nodes.push_back(node);
			if (node->get_c())
				for (auto ta : node->get_c()->tasks)
					modules_set.insert(&ta->get_module());
			for (auto c : node->get_children())
				collect_modules_list(c, already_parsed_nodes);
		}
	};
	std::vector<const Generic_node<tools::Sub_sequence>*> already_parsed_nodes;
	collect_modules_list(root, already_parsed_nodes);

	for (auto m : modules_set)
		this->all_modules[0].push_back(m);

	this->duplicate<tools::Sub_sequence, module::Module>(root);
}

size_t Sequence
::get_n_frames() const
{
	const auto n_frames = this->all_modules[0][0]->get_n_frames();

	for (auto &mm : this->all_modules)
		for (auto &m : mm)
			if (m->get_n_frames() != n_frames)
			{
				std::stringstream message;
				message << "All the modules do not have the same 'n_frames' value ('m->get_n_frames()' = "
				        << m->get_n_frames() << ", 'n_frames' = " << n_frames << ").";
				throw tools::runtime_error(__FILE__, __LINE__, __func__, message.str());
			}

	return n_frames;
}

}
}
