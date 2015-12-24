#if !NDEBUG
#ifndef DEBUG_H
#define DEBUG_H

namespace flare {
    namespace debug {

	void entityTree();
	void entityTreeNode(fuse::Entity *entity, bool showChildren = false);
    }
}

#endif
#endif // !NDEBUG
