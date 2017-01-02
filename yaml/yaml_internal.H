#ifndef yaml_internal_H
#define yaml_internal_H

class LIBCXX_HIDDEN docnodeObj::rootnodeObj : public docnodeObj {

 public:
	rootnodeObj(const document &docArg);
	~rootnodeObj();

	const yaml_node_t *getNode(const documentObj::readlock_t &lock)
		override;

	yaml_node_t *getNode(const documentObj::writelock_t &lock) override;
};

class LIBCXX_HIDDEN docnodeObj::nonrootnodeObj : public docnodeObj {
 public:

	yaml_node_item_t item;

	nonrootnodeObj(const document &docArg, yaml_node_item_t itemArg);
	~nonrootnodeObj();

	const yaml_node_t *getNode(const documentObj::readlock_t &lock)
		override;

	yaml_node_t *getNode(const documentObj::writelock_t &lock) override;
};

#endif
