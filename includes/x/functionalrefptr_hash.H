#ifdef x_functionalrefptrobj_H_defined
#ifdef x_refptr_hash_H_defined

namespace std {

	//! Implemented std::hashfor functionref and functionptr

	template<typename base_ref,
		 typename return_type,
		 typename ...Args>
	struct hash<LIBCXX_NAMESPACE::functionrefptr_impl<base_ref,
							  return_type(Args...)>
		    > : hash<base_ref> {

		using hash<base_ref>::operator();
	};
}

#endif
#endif
