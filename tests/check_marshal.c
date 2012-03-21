#include <stdlib.h>
#include <unistd.h>
#include <check.h>
#include <sys/queue.h>

#define MARSHAL_EXPORT
#include "../src/marshal.h"

/* This suite can be run in valgrind for memory leaks:
   CK_FORK=no valgrind -v --leak-check=yes ./tests/check_marshal
*/


struct struct_simple {
	int a1;
	long a2;
	char a3;
	time_t a4;
	char a5[7];
};
MARSHAL(struct_simple);

START_TEST(test_simple_structure) {
	struct struct_simple source = {
		.a1 = 78452,
		.a2 = 48751424,
		.a3 = 'h',
		.a4 = 784254,
		.a5 = { 'A', 'B', 'C', 'D', 'E', 'F', 'G'},
	};
	struct struct_simple *destination;
	void *buffer;
	size_t len, len2;

	len = marshal_serialize(struct_simple, &source, &buffer);
	fail_unless(len > 0, "Unable to serialize");
	memset(&source, 0, sizeof(struct struct_simple));
	len2 = marshal_unserialize(struct_simple, buffer, len, &destination);
	fail_unless(len2 > 0, "Unable to deserialize");
	free(buffer);
	ck_assert_int_eq(len, len2);
	ck_assert_int_eq(destination->a1, 78452);
	ck_assert_int_eq(destination->a2, 48751424);
	ck_assert_int_eq(destination->a3, 'h');
	ck_assert_int_eq(destination->a4, 784254);
	ck_assert_int_eq(destination->a5[0], 'A');
	ck_assert_int_eq(destination->a5[1], 'B');
	ck_assert_int_eq(destination->a5[2], 'C');
	ck_assert_int_eq(destination->a5[3], 'D');
	ck_assert_int_eq(destination->a5[4], 'E');
	ck_assert_int_eq(destination->a5[5], 'F');
	ck_assert_int_eq(destination->a5[6], 'G');
	free(destination);
}
END_TEST

struct struct_sub {
	int e1;
	struct struct_simple e2;
	char e3;
};
MARSHAL_BEGIN(struct_sub)
MARSHAL_SUBSTRUCT(struct_sub, struct_simple, e2)
MARSHAL_END;

START_TEST(test_substruct_structure) {
	struct struct_sub source = {
		.e1 = -5122,
		.e2 = {
			.a1 = 78452,
			.a2 = 48751424,
			.a3 = 'h',
			.a4 = 784254,
			.a5 = { 'A', 'B', 'C', 'D', 'E', 'F', 'G'},
		},
		.e3 = 'a',
	};

	struct struct_sub *destination;
	void *buffer;
	size_t len, len2;

	len = marshal_serialize(struct_sub, &source, &buffer);
	fail_unless(len > 0, "Unable to serialize");
	memset(&source, 0, sizeof(struct struct_sub));
	len2 = marshal_unserialize(struct_sub, buffer, len, &destination);
	fail_unless(len2 > 0, "Unable to deserialize");
	free(buffer);
	ck_assert_int_eq(len, len2);
	ck_assert_int_eq(destination->e1, -5122);
	ck_assert_int_eq(destination->e2.a1, 78452);
	ck_assert_int_eq(destination->e2.a2, 48751424);
	ck_assert_int_eq(destination->e2.a3, 'h');
	ck_assert_int_eq(destination->e2.a4, 784254);
	ck_assert_int_eq(destination->e2.a5[0], 'A');
	ck_assert_int_eq(destination->e2.a5[1], 'B');
	ck_assert_int_eq(destination->e2.a5[2], 'C');
	ck_assert_int_eq(destination->e2.a5[3], 'D');
	ck_assert_int_eq(destination->e2.a5[4], 'E');
	ck_assert_int_eq(destination->e2.a5[5], 'F');
	ck_assert_int_eq(destination->e2.a5[6], 'G');
	ck_assert_int_eq(destination->e3, 'a');
	free(destination);
}
END_TEST

struct struct_onepointer {
	int b1;
	long b2;
	char b3;
	struct struct_simple *b4;
	int b5;
};
MARSHAL_BEGIN(struct_onepointer)
MARSHAL_POINTER(struct_onepointer, struct_simple, b4)
MARSHAL_END;

START_TEST(test_pointer_structure) {
	struct struct_simple source_simple = {
		.a1 = 78452,
		.a2 = 48751424,
		.a3 = 'h',
		.a4 = 784254,
		.a5 = { 'A', 'B', 'C', 'D', 'E', 'F', 'G'},
	};
	struct struct_onepointer source = {
		.b1 = 18,
		.b2 = 15454,
		.b3 = 'o',
		.b4 = &source_simple,
		.b5 = 333333,
	};

	struct struct_onepointer *destination;
	void *buffer;
	size_t len, len2;

	len = marshal_serialize(struct_onepointer, &source, &buffer);
	fail_unless(len > 0, "Unable to serialize");
	memset(&source_simple, 0, sizeof(struct struct_simple));
	memset(&source, 0, sizeof(struct struct_onepointer));
	len2 = marshal_unserialize(struct_onepointer, buffer, len, &destination);
	fail_unless(len2 > 0, "Unable to deserialize");
	free(buffer);
	ck_assert_int_eq(len, len2);
	ck_assert_int_eq(destination->b1, 18);
	ck_assert_int_eq(destination->b2, 15454);
	ck_assert_int_eq(destination->b3, 'o');
	ck_assert_int_eq(destination->b4->a1, 78452);
	ck_assert_int_eq(destination->b4->a2, 48751424);
	ck_assert_int_eq(destination->b4->a3, 'h');
	ck_assert_int_eq(destination->b4->a4, 784254);
	ck_assert_int_eq(destination->b4->a5[0], 'A');
	ck_assert_int_eq(destination->b4->a5[1], 'B');
	ck_assert_int_eq(destination->b4->a5[2], 'C');
	ck_assert_int_eq(destination->b4->a5[3], 'D');
	ck_assert_int_eq(destination->b4->a5[4], 'E');
	ck_assert_int_eq(destination->b4->a5[5], 'F');
	ck_assert_int_eq(destination->b4->a5[6], 'G');
	ck_assert_int_eq(destination->b5, 333333);
	free(destination->b4); free(destination);
}
END_TEST

struct struct_nestedpointers {
	int c1;
	long c2;
	struct struct_simple *c3;
	struct struct_onepointer *c4;
	int c5;
};
MARSHAL_BEGIN(struct_nestedpointers)
MARSHAL_POINTER(struct_nestedpointers, struct_simple, c3)
MARSHAL_POINTER(struct_nestedpointers, struct_onepointer, c4)
MARSHAL_END;

START_TEST(test_several_pointers_structure) {
	struct struct_simple source_simple1 = {
		.a1 = 78452,
		.a2 = 48751424,
		.a3 = 'h',
		.a4 = 784254,
		.a5 = { 'A', 'B', 'C', 'D', 'E', 'F', 'G'},
	};
	struct struct_simple source_simple2 = {
		.a1 = 451,
		.a2 = 451424,
		.a3 = 'o',
		.a4 = 74,
		.a5 = { 'a', 'b', 'c', 'd', 'e', 'f', 'g'},
	};
	struct struct_onepointer source_onepointer = {
		.b1 = 18,
		.b2 = 15454,
		.b3 = 'o',
		.b4 = &source_simple1,
		.b5 = 333333,
	};
	struct struct_nestedpointers source = {
		.c1 = 4542,
		.c2 = 5665454,
		.c3 = &source_simple2,
		.c4 = &source_onepointer,
		.c5 = -545424,
	};

	struct struct_nestedpointers *destination;
	void *buffer;
	size_t len, len2;

	len = marshal_serialize(struct_nestedpointers, &source, &buffer);
	fail_unless(len > 0, "Unable to serialize");
	memset(&source_simple1, 0, sizeof(struct struct_simple));
	memset(&source_simple2, 0, sizeof(struct struct_simple));
	memset(&source_onepointer, 0, sizeof(struct struct_onepointer));
	memset(&source, 0, sizeof(struct struct_nestedpointers));
	len2 = marshal_unserialize(struct_nestedpointers, buffer, len, &destination);
	fail_unless(len2 > 0, "Unable to deserialize");
	free(buffer);
	ck_assert_int_eq(len, len2);
	ck_assert_int_eq(destination->c1, 4542);
	ck_assert_int_eq(destination->c2, 5665454);
	ck_assert_int_eq(destination->c3->a1, 451);
	ck_assert_int_eq(destination->c3->a2, 451424);
	ck_assert_int_eq(destination->c3->a3, 'o');
	ck_assert_int_eq(destination->c3->a4, 74);
	ck_assert_int_eq(destination->c3->a5[3], 'd');
	ck_assert_int_eq(destination->c3->a5[4], 'e');
	ck_assert_int_eq(destination->c3->a5[6], 'g');
	ck_assert_int_eq(destination->c4->b1, 18);
	ck_assert_int_eq(destination->c4->b2, 15454);
	ck_assert_int_eq(destination->c4->b3, 'o');
	ck_assert_int_eq(destination->c4->b4->a1, 78452);
	ck_assert_int_eq(destination->c4->b4->a2, 48751424);
	ck_assert_int_eq(destination->c4->b4->a3, 'h');
	ck_assert_int_eq(destination->c4->b4->a4, 784254);
	ck_assert_int_eq(destination->c4->b4->a5[0], 'A');
	ck_assert_int_eq(destination->c4->b4->a5[1], 'B');
	ck_assert_int_eq(destination->c4->b4->a5[2], 'C');
	ck_assert_int_eq(destination->c4->b4->a5[3], 'D');
	ck_assert_int_eq(destination->c4->b4->a5[4], 'E');
	ck_assert_int_eq(destination->c4->b4->a5[5], 'F');
	ck_assert_int_eq(destination->c4->b4->a5[6], 'G');
	ck_assert_int_eq(destination->c4->b5, 333333);	
	free(destination->c3); free(destination->c4->b4);
	free(destination->c4); free(destination);
}
END_TEST

START_TEST(test_null_pointers) {
	struct struct_simple source_simple2 = {
		.a1 = 451,
		.a2 = 451424,
		.a3 = 'o',
		.a4 = 74,
		.a5 = { 'a', 'b', 'c', 'd', 'e', 'f', 'g'},
	};
	struct struct_nestedpointers source = {
		.c1 = 4542,
		.c2 = 5665454,
		.c3 = &source_simple2,
		.c4 = NULL,
		.c5 = -545424,
	};

	struct struct_nestedpointers *destination;
	void *buffer;
	size_t len, len2;

	len = marshal_serialize(struct_nestedpointers, &source, &buffer);
	fail_unless(len > 0, "Unable to serialize");
	memset(&source_simple2, 0, sizeof(struct struct_simple));
	memset(&source, 0, sizeof(struct struct_nestedpointers));
	len2 = marshal_unserialize(struct_nestedpointers, buffer, len, &destination);
	fail_unless(len2 > 0, "Unable to deserialize");
	free(buffer);
	ck_assert_int_eq(len, len2);
	ck_assert_int_eq(destination->c1, 4542);
	ck_assert_int_eq(destination->c2, 5665454);
	ck_assert_int_eq(destination->c3->a1, 451);
	ck_assert_int_eq(destination->c3->a2, 451424);
	ck_assert_int_eq(destination->c3->a3, 'o');
	ck_assert_int_eq(destination->c3->a4, 74);
	ck_assert_int_eq(destination->c3->a5[3], 'd');
	ck_assert_int_eq(destination->c3->a5[4], 'e');
	ck_assert_int_eq(destination->c3->a5[6], 'g');
	ck_assert_int_eq(destination->c4, NULL);
	free(destination->c3); free(destination);	
}
END_TEST

struct struct_multipleref {
	int f1;
	struct struct_simple* f2;
	struct struct_simple* f3;
	struct struct_nestedpointers* f4;
};
MARSHAL_BEGIN(struct_multipleref)
MARSHAL_POINTER(struct_multipleref, struct_simple, f2)
MARSHAL_POINTER(struct_multipleref, struct_simple, f3)
MARSHAL_POINTER(struct_multipleref, struct_nestedpointers, f4)
MARSHAL_END;

START_TEST(test_multiple_references) {
	struct struct_simple source_simple = {
		.a1 = 451,
		.a2 = 451424,
		.a3 = 'o',
		.a4 = 74,
		.a5 = { 'a', 'b', 'c', 'd', 'e', 'f', 'g'},
	};
	struct struct_nestedpointers source_nested = {
		.c3 = &source_simple,
		.c4 = NULL,
	};
	struct struct_multipleref source = {
		.f1 = 15,
		.f2 = &source_simple,
		.f3 = &source_simple,
		.f4 = &source_nested,
	};

	struct struct_multipleref *destination;
	void *buffer = NULL;
	size_t len, len2;

	len = marshal_serialize(struct_multipleref, &source, &buffer);
	fail_unless(buffer != NULL, "Buffer is empty");
	fail_unless(len > 0, "Unable to serialize");
	memset(&source_simple, 0, sizeof(struct struct_simple));
	memset(&source_nested, 0, sizeof(struct struct_nestedpointers));
	memset(&source, 0, sizeof(struct struct_multipleref));
	len2 = marshal_unserialize(struct_multipleref, buffer, len, &destination);
	fail_unless(len2 > 0, "Unable to deserialize");
	free(buffer);
	ck_assert_int_eq(len, len2);
	ck_assert_int_eq(destination->f1, 15);
	ck_assert_int_eq(destination->f2, destination->f3);
	ck_assert_int_eq(destination->f2, destination->f4->c3);
	ck_assert_int_eq(destination->f2->a1, 451);
	ck_assert_int_eq(destination->f2->a2, 451424);
	ck_assert_int_eq(destination->f2->a3, 'o');
	ck_assert_int_eq(destination->f2->a4, 74);
	ck_assert_int_eq(destination->f4->c4, NULL);
	free(destination->f2); free(destination->f4); free(destination);
}
END_TEST

struct struct_circularref {
	int g1;
	struct struct_circularref* g2;
};
MARSHAL_BEGIN(struct_circularref)
MARSHAL_POINTER(struct_circularref, struct_circularref, g2)
MARSHAL_END;

START_TEST(test_circular_references) {
	struct struct_circularref source = {
		.g1 = 42,
		.g2 = &source,
	};

	struct struct_circularref *destination;
	void *buffer = NULL;
	size_t len, len2;

	len = marshal_serialize(struct_circularref, &source, &buffer);
	fail_unless(len > 0, "Unable to serialize");
	memset(&source, 0, sizeof(struct struct_circularref));
	len2 = marshal_unserialize(struct_circularref, buffer, len, &destination);
	fail_unless(len2 > 0, "Unable to deserialize");
	free(buffer);
	ck_assert_int_eq(len, len2);
	ck_assert_int_eq(destination->g1, 42);
	ck_assert_int_eq(destination->g2->g1, 42);
	ck_assert_int_eq(destination->g2, destination->g2->g2);
	free(destination);
}
END_TEST

START_TEST(test_too_small_unmarshal) {
	struct struct_simple source_simple1;
	struct struct_onepointer source_onepointer = {
		.b4 = &source_simple1,
	};
	struct struct_nestedpointers source = {
		.c3 = &source_simple1,
		.c4 = &source_onepointer,
	};

	struct struct_nestedpointers *destination;
	void *buffer;
	size_t len, len2;

	len = marshal_serialize(struct_nestedpointers, &source, &buffer);
	fail_unless(len > 0, "Unable to serialize");
	memset(&source_simple1, 0, sizeof(struct struct_simple));
	memset(&source_onepointer, 0, sizeof(struct struct_onepointer));
	memset(&source, 0, sizeof(struct struct_nestedpointers));
	int i, j;
	/* Loop 30 times to ease debugging leaks with valgrind */
	for (j = 0; j < 30; j++) {
		for (i = 0; i < len; i++) {
			len2 = marshal_unserialize(struct_nestedpointers, buffer, 1, &destination);
			fail_unless(len2 == 0,
				    "Should not be able to deserialize, too small (%d<%d)",
				    i, len);
		}
	}
	len2 = marshal_unserialize(struct_nestedpointers, buffer, len + 5, &destination);
	fail_unless(len2 == len, "Deserialized too much");
	free(destination->c3);
	free(destination->c4); free(destination); free(buffer);
}
END_TEST

struct struct_simpleentry {
	TAILQ_ENTRY(struct_simpleentry) s_entries;
	int g1;
	struct struct_simple *g2;
};
MARSHAL_BEGIN(struct_simpleentry)
MARSHAL_TQE(struct_simpleentry, s_entries)
MARSHAL_POINTER(struct_simpleentry, struct_simple, g2)
MARSHAL_END;

TAILQ_HEAD(list_simple, struct_simpleentry);
MARSHAL_TQ(list_simple, struct_simpleentry);

START_TEST(test_simple_list) {
	struct struct_simple source_simple = {
		.a1 = 451,
		.a2 = 451424,
		.a3 = 'o',
		.a4 = 74,
		.a5 = { 'a', 'b', 'c', 'd', 'e', 'f', 'g'},
	};
	struct list_simple source;
	struct struct_simpleentry entry1 = {
		.g1 = 47,
		.g2 = &source_simple,
	};
	struct struct_simpleentry entry2 = {
		.g1 = 49,
		.g2 = &source_simple,
	};
	struct struct_simpleentry entry3 = {
		.g1 = 4700,
		.g2 = NULL,
	};
	struct struct_simpleentry entry4 = {
		.g1 = -47,
		.g2 = &source_simple,
	};
	TAILQ_INIT(&source);
	TAILQ_INSERT_TAIL(&source, &entry1, s_entries);
	TAILQ_INSERT_TAIL(&source, &entry2, s_entries);
	TAILQ_INSERT_TAIL(&source, &entry3, s_entries);
	TAILQ_INSERT_TAIL(&source, &entry4, s_entries);

	struct list_simple *destination;
	void *buffer;
	size_t len, len2;
	len = marshal_serialize(list_simple, &source, &buffer);
	fail_unless(len > 0, "Unable to serialize");
	memset(&source, 0, sizeof(struct list_simple));
	memset(&entry1, 0, sizeof(struct struct_simpleentry));
	memset(&entry2, 0, sizeof(struct struct_simpleentry));
	memset(&entry3, 0, sizeof(struct struct_simpleentry));
	memset(&entry4, 0, sizeof(struct struct_simpleentry));
	len2 = marshal_unserialize(list_simple, buffer, len, &destination);
	fail_unless(len2 > 0, "Unable to deserialize");
	free(buffer);

	struct struct_simpleentry *e1, *e2;
	struct struct_simple *s;
	e1 = TAILQ_FIRST(destination);
	ck_assert_int_eq(e1->g1, 47);
	s = e1->g2;
	e2 = TAILQ_NEXT(e1, s_entries);
	free(e1);
	ck_assert_int_eq(e2->g1, 49);
	ck_assert_int_eq(e2->g2, s);
	e1 = TAILQ_NEXT(e2, s_entries);
	free(e2);
	ck_assert_int_eq(e1->g1, 4700);
	ck_assert_int_eq(e1->g2, NULL);
	e2 = TAILQ_NEXT(e1, s_entries);
	free(e1);
	ck_assert_int_eq(e2->g1, -47);
	ck_assert_int_eq(e2->g2, s);
	e1 = TAILQ_NEXT(e2, s_entries);
	free(e2);
	ck_assert_int_eq(e1, NULL);
	free(s);
	free(destination);
}
END_TEST

struct struct_withlist {
	int i1;
	TAILQ_HEAD(, struct_simpleentry) i2;
	int i3;
};
MARSHAL_BEGIN(struct_withlist)
MARSHAL_SUBTQ(struct_withlist, struct_simpleentry, i2)
MARSHAL_END;

START_TEST(test_embedded_list) {
	struct struct_withlist source = {
		.i1 = 45424,
		.i3 = 4542,
	};
	struct struct_simple source_simple = {
		.a1 = 451,
		.a2 = 451424,
		.a3 = 'o',
		.a4 = 74,
		.a5 = { 'a', 'b', 'c', 'd', 'e', 'f', 'g'},
	};
	struct struct_simpleentry entry1 = {
		.g1 = 47,
		.g2 = &source_simple,
	};
	struct struct_simpleentry entry2 = {
		.g1 = 49,
		.g2 = &source_simple,
	};
	struct struct_simpleentry entry3 = {
		.g1 = 4700,
		.g2 = NULL,
	};
	struct struct_simpleentry entry4 = {
		.g1 = -47,
		.g2 = &source_simple,
	};
	TAILQ_INIT(&source.i2);
	TAILQ_INSERT_TAIL(&source.i2, &entry1, s_entries);
	TAILQ_INSERT_TAIL(&source.i2, &entry2, s_entries);
	TAILQ_INSERT_TAIL(&source.i2, &entry3, s_entries);
	TAILQ_INSERT_TAIL(&source.i2, &entry4, s_entries);

	struct struct_withlist *destination;
	void *buffer;
	size_t len, len2;
	len = marshal_serialize(struct_withlist, &source, &buffer);
	fail_unless(len > 0, "Unable to serialize");
	memset(&source, 0, sizeof(struct list_simple));
	memset(&entry1, 0, sizeof(struct struct_simpleentry));
	memset(&entry2, 0, sizeof(struct struct_simpleentry));
	memset(&entry3, 0, sizeof(struct struct_simpleentry));
	memset(&entry4, 0, sizeof(struct struct_simpleentry));
	len2 = marshal_unserialize(struct_withlist, buffer, len, &destination);
	fail_unless(len2 > 0, "Unable to deserialize");
	free(buffer);

	ck_assert_int_eq(destination->i1, 45424);
	ck_assert_int_eq(destination->i3, 4542);
	struct struct_simpleentry *e1, *e2;
	struct struct_simple *s;
	e1 = TAILQ_FIRST(&destination->i2);
	ck_assert_int_eq(e1->g1, 47);
	ck_assert_int_eq(e1->g2->a4, 74);
	s = e1->g2;
	e2 = TAILQ_NEXT(e1, s_entries);
	free(e1);
	ck_assert_int_eq(e2->g1, 49);
	ck_assert_int_eq(e2->g2, s);
	e1 = TAILQ_NEXT(e2, s_entries);
	free(e2);
	ck_assert_int_eq(e1->g1, 4700);
	ck_assert_int_eq(e1->g2, NULL);
	e2 = TAILQ_NEXT(e1, s_entries);
	free(e1);
	ck_assert_int_eq(e2->g1, -47);
	ck_assert_int_eq(e2->g2, s);
	e1 = TAILQ_NEXT(e2, s_entries);
	free(e2);
	ck_assert_int_eq(e1, NULL);
	free(s);
	free(destination);
}
END_TEST

struct struct_string {
	int s1;
	char *s2;
	char *s3;
};
MARSHAL_BEGIN(struct_string)
MARSHAL_STR(struct_string, s2)
MARSHAL_STR(struct_string, s3)
MARSHAL_END;

START_TEST(test_string) {
	struct struct_string source = {
		.s1 = 44444,
		.s2 = "String 2",
		.s3 = "String 3",
	};
	struct struct_string *destination;
	void *buffer;
	size_t len, len2;

	len = marshal_serialize(struct_string, &source, &buffer);
	fail_unless(len > 0, "Unable to serialize");
	memset(&source, 0, sizeof(struct struct_string));
	len2 = marshal_unserialize(struct_string, buffer, len, &destination);
	fail_unless(len2 > 0, "Unable to deserialize");
	free(buffer);
	ck_assert_int_eq(len, len2);
	ck_assert_int_eq(destination->s1, 44444);
	ck_assert_str_eq(destination->s2, "String 2");
	ck_assert_str_eq(destination->s3, "String 3");
	free(destination->s2); free(destination->s3);
	free(destination);
}
END_TEST

struct struct_fixedstring {
	int s1;
	char *s2;
	int s2_len;
	char *s3;
};
MARSHAL_BEGIN(struct_fixedstring)
MARSHAL_FSTR(struct_fixedstring, s2, s2_len)
MARSHAL_STR(struct_fixedstring, s3)
MARSHAL_END;

START_TEST(test_fixed_string) {
	struct struct_fixedstring source = {
		.s1 = 44444,
		.s2 = "String 2 Bla",
		.s2_len = 8,
		.s3 = "String 3",
	};
	struct struct_fixedstring *destination;
	void *buffer;
	size_t len, len2;

	len = marshal_serialize(struct_fixedstring, &source, &buffer);
	fail_unless(len > 0, "Unable to serialize");
	memset(&source, 0, sizeof(struct struct_fixedstring));
	len2 = marshal_unserialize(struct_fixedstring, buffer, len, &destination);
	fail_unless(len2 > 0, "Unable to deserialize");
	free(buffer);
	ck_assert_int_eq(len, len2);
	ck_assert_int_eq(destination->s1, 44444);
	ck_assert_int_eq(destination->s2_len, 8);
	ck_assert_int_eq(destination->s2[0], 'S');
	ck_assert_int_eq(destination->s2[2], 'r');
	ck_assert_int_eq(destination->s2[4], 'n');
	ck_assert_int_eq(destination->s2[5], 'g');
	ck_assert_int_eq(destination->s2[6], ' ');
	ck_assert_int_eq(destination->s2[7], '2');
	ck_assert_str_eq(destination->s3, "String 3");
	free(destination->s2); free(destination->s3);
	free(destination);
}
END_TEST

struct struct_ignore {
	int t1;
	void *t2;
	int t3;
};
MARSHAL_BEGIN(struct_ignore)
MARSHAL_IGNORE(struct_ignore, t2)
MARSHAL_END;

START_TEST(test_ignore) {
	struct struct_ignore source = {
		.t1 = 4544,
		.t2 = (void *)"String 2 Bla",
		.t3 = 11111,
	};
	struct struct_ignore *destination;
	void *buffer;
	size_t len, len2;

	len = marshal_serialize(struct_ignore, &source, &buffer);
	fail_unless(len > 0, "Unable to serialize");
	memset(&source, 0, sizeof(struct struct_ignore));
	len2 = marshal_unserialize(struct_ignore, buffer, len, &destination);
	fail_unless(len2 > 0, "Unable to deserialize");
	free(buffer);
	ck_assert_int_eq(len, len2);
	ck_assert_int_eq(destination->t1, 4544);
	ck_assert_int_eq(destination->t2, NULL);
	ck_assert_int_eq(destination->t3, 11111);
	free(destination);
}
END_TEST

Suite *
marshal_suite(void)
{
	Suite *s = suite_create("Marshalling");

	TCase *tc_marshal = tcase_create("Marshalling");
	tcase_add_test(tc_marshal, test_simple_structure);
	tcase_add_test(tc_marshal, test_substruct_structure);
	tcase_add_test(tc_marshal, test_pointer_structure);
	tcase_add_test(tc_marshal, test_several_pointers_structure);
	tcase_add_test(tc_marshal, test_null_pointers);
	tcase_add_test(tc_marshal, test_multiple_references);
	tcase_add_test(tc_marshal, test_circular_references);
	tcase_add_test(tc_marshal, test_too_small_unmarshal);
	tcase_add_test(tc_marshal, test_simple_list);
	tcase_add_test(tc_marshal, test_embedded_list);
	tcase_add_test(tc_marshal, test_string);
	tcase_add_test(tc_marshal, test_fixed_string);
	tcase_add_test(tc_marshal, test_ignore);
	suite_add_tcase(s, tc_marshal);

	return s;
}

int
main()
{
	int number_failed;
	Suite *s = marshal_suite();
	SRunner *sr = srunner_create(s);
	srunner_run_all(sr, CK_ENV);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
