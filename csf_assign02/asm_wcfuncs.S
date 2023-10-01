/*
 * Assembly language function implementations
 */

#define MAX_WORDLEN 63

/*
 * Offsets for the fields of the struct WordEntry data type
 * (Note that there is 4 bytes of padding between the count and next
 * fields.)
 */
#define WORDENTRY_WORD_OFFSET   (0)
#define WORDENTRY_COUNT_OFFSET  (MAX_WORDLEN+1)
#define WORDENTRY_NEXT_OFFSET   (MAX_WORDLEN+1+4+4)

	.section .rodata
/* Define any string constants or read-only data here */

	.section .text

/*
 * Sebastian Rincon
 * Compute a hash code for the given NUL-terminated
 * character string.
 *
 * The hash algorithm should be implemented as follows:
 *
 * uint32_t hash_code = 5381
 * for each character c of w in order {
 *   hash_code = hash_code * 33 + c
 * }
 *
 * Note that the character values should be treated as
 * being unsigned (in the range 0..255)
 *
 * C function prototype:
 *    uint32_t wc_hash(const unsigned char *w);
 */
	.globl wc_hash
wc_hash:
	subq $8, %rsp
	movl $5381, %eax  /* make rax the hashcode 5381*/
	movb $0, %r10b /*instantiates the local variable for byte to 0*/
	.Lmain_loop:
		movzbl (%edi), %r10d /* load char from edi into r10d */
		testb %r10b, %r10b /*check char with itself to see if it is null*/
		jz .Ldone   /*jump to end to return hashcode*/
		imull $33, %eax /* unsinged multiplication of eax with 33 into eax */
		addl %r10d, %eax  /* adds the unsinged char value to eax*/
		incb %dil /*incraments pointer of string to next byte*/
		jmp .Lmain_loop /*jumps back to the beginning of the loop*/
	.Ldone:
		addq $8, %rsp  /* realligns stack pointer*/
		ret  /* return eax */

/*
 * Compare two strings lexicographically. Return
 *
 * - a negative value if lhs string is less than rhs string
 * - 0 if lhs string is identical to rhs string
 * - a positive value if lhs string is greater than rhs string
 *
 * Lexicographical comparison is a generalization of alphabetical
 * order, but using character codes. If one string is a prefix
 * of the other, it is considered as "less than". E.g.,
 * "hi" would compare as less than "high".
 *
 * C function prototype:
 *    int wc_str_compare(const unsigned char *lhs, const unsigned char *rhs);
 */
	.globl wc_str_compare
wc_str_compare:
	/* Begin */
	push   %rbp                /* Save old base pointer */
	mov    %rbp, %rsp          /* Set new base pointer */

.loop:
	/* Load a byte from lhs into al register */
	mov    (%rdi), %al         /* Load byte from address in rdi to al */

	/* Load a byte from rhs into dl register */
	mov    (%rsi), %dl         /* Load byte from address in rsi to dl */

	/* Check if *lhs != *rhs */
	cmp    %dl, %al            /* Compare dl and al */
	jne    .doneCompare               /* If not equal, exit the loop */

	/* Check if *lhs == 0, i.e., end of lhs string */
	test   %al, %al            /* Test al against itself */
	je     .doneCompare               /* If zero, exit the loop */

	/* Move to the next characters */
	inc    %rdi                /* Move to next byte of lhs */
	inc    %rsi                /* Move to next byte of rhs */
	jmp    .loop               /* Jump back to the start of the loop */

.doneCompare:
	/* Return *(const unsigned char *)lhs - *(const unsigned char *)rhs */
	sub    %dl, %al            /* Subtract dl (rhs byte) from al (lhs byte) */
	movzx  %al, %eax           /* Zero-extend al to eax for the return value */

	/* end and clean */
	leave                      /* Cleanup stack and restore base pointer */
	ret                        /* Return with difference in eax */


/*
 * Copy NUL-terminated source string to the destination buffer.
 *
 * C function prototype:
 *    void wc_str_copy(unsigned char *dest, const unsigned char *source);
 */
	.globl wc_str_copy
wc_str_copy:
	/* Entry */
	push   %rbp                /* Save old base pointer */
	mov    %rbp, %rsp          /* Set new base pointer */

	/* Begin copying loop */
.copy_loop:
	/* Load a byte from source into al register */
	mov    (%rsi), %al         /* Load byte from address in rsi (source) to al */

	/* Store the byte in destination */
	mov    %al, (%rdi)         /* Store byte from al to address in rdi (destination) */

	/* Check if we have hit the NUL terminator */
	test   %al, %al            /* Test al against itself */
	je     .end_copy           /* If zero (NUL terminator), exit the loop */

	/* Move to the next characters */
	inc    %rdi                /* Move to next byte of destination */
	inc    %rsi                /* Move to next byte of source */
	jmp    .copy_loop          /* Jump back to the start of the loop */

.end_copy:
	leave                      /* Cleanup stack and restore base pointer */
	ret                        /* Return */

/*
 * Return 1 if the character code in c is a whitespace character,
 * false otherwise.
 *
 * For the purposes of this function, a whitespace character is one of
 *
 *   ' '
 *   '\t'
 *   '\r'
 *   '\n'
 *   '\f'
 *   '\v'
 *
 * C function prototype:
 *    int wc_isspace(unsigned char c);
 */
	.globl wc_isspace
wc_isspace:
    /* rdi = unsigned char c (as this is the first integer/pointer argument) */

    /* Compare c with ' ' */
    cmpb $0x20, %dil         	/* Compare lower byte of rdi with ASCII for space */
    je .is_space           		/* If equal, it's a whitespace */

    /* Compare c with '\t' */
    cmpb $0x09, %dil         	/* Compare with ASCII for tab */
    je .is_space           		/* If equal, it's a whitespace */

    /* Compare c with '\r' */
    cmpb $0x0d, %dil         	/* Compare with ASCII for carriage return */
    je .is_space           		/* If equal, it's a whitespace */

    /* Compare c with '\n' */
    cmpb $0x0a, %dil         	/* Compare with ASCII for newline */
    je .is_space           		/* If equal, it's a whitespace */

    /* Compare c with '\f' */
    cmpb $0x0c, %dil         	/* Compare with ASCII for form feed */
    je .is_space           		/* If equal, it's a whitespace */

    /* Compare c with '\v' */
    cmpb $0x0b, %dil         	/* Compare with ASCII for vertical tab */
    je .is_space           		/* If equal, it's a whitespace */

    /* Not a whitespace character */
    mov	$0, %eax          		/* Set return value to 0 */
    ret

.is_space:
    /* It's a whitespace character */
    mov $1, %eax            	/* Set return value to 1 */
    ret

/*
 * Return 1 if the character code in c is an alphabetic character
 * ('A' through 'Z' or 'a' through 'z'), 0 otherwise.
 *
 * C function prototype:
 *    int wc_isalpha(unsigned char c);
 */
	.globl wc_isalpha
wc_isalpha:
    /* rdi = unsigned char c (as this is the first integer/pointer argument) */

    /* First, check if it's an uppercase letter */
    cmpb   $'A', %dil          /* Compare the character with 'A' */
    jl     .not_alpha          /* If less than 'A', not an alphabetic character */

    cmpb   $'Z', %dil          /* Compare the character with 'Z' */
    jle    .is_alpha           /* If less than or equal to 'Z', it's an uppercase alphabetic character */

    /* Now, check if it's a lowercase letter */
    cmpb   $'a', %dil          /* Compare the character with 'a' */
    jl     .not_alpha          /* If less than 'a', not an alphabetic character */

    cmpb   $'z', %dil          /* Compare the character with 'z' */
    jg     .not_alpha          /* If greater than 'z', not an alphabetic character */

.is_alpha:
    mov    $1, %eax            /* Set return value to 1 (it's an alphabetic character) */
    ret

.not_alpha:
    mov		$0, %eax       		/* Set return value to 0 (not an alphabetic character) */
    ret


/*
 * Read the next word from given input stream, storing
 * the word in the array pointed-to by w. (This array should be
 * assumed to be MAX_WORDLEN+1 elements in size.) Return 1
 * if a word is read successfully, 0 otherwise.
 *
 * For the purposes of this function, a word is a sequence of
 * 1 or more non-whitespace characters.
 *
 * If a sequence of non-whitespace characters has more than
 * MAX_WORDLEN characters, then only the first MAX_WORDLEN
 * characters in the sequence should be stored in the array.
 *
 * C function prototype:
 *    int wc_readnext(FILE *in, unsigned char *w);
 */

	.globl wc_readnext
wc_readnext:
	/* TODO: implement */
	ret

/*
 * Convert the NUL-terminated character string in the array
 * pointed-to by w so that every letter is lower-case.
 *
 * C function prototype:
 *    void wc_tolower(unsigned char *w);
 */
	.globl wc_tolower
str_tolower:
	subq $8, %rsp

.Lstr_tolower_loop:
	movb (%rdi), %al
	cmpb $0, %al
	je .Lstr_tolower_done

	cmpb $65, %al
	jb .Lstr_tolower_loop_continue
	cmpb $90, %al
	ja .Lstr_tolower_loop_continue

	addb $(97 - 65), %al
	movb %al, (%rdi)

.Lstr_tolower_loop_continue:
	incq %rdi
	jmp .Lstr_tolower_loop

.Lstr_tolower_done:
	addq $8, %rsp
	ret

/*
 * Remove any non-alphaabetic characters from the end of the
 * NUL-terminated character string pointed-to by w.
 *
 * C function prototype:
 *    void wc_trim_non_alpha(unsigned char *w);
 */
    .globl wc_trim_non_alpha
wc_trim_non_alpha:
    /* Entry */
    push   %rbp                /* Save old base pointer */
    mov    %rbp, %rsp          /* Set new base pointer */
    push   %rdi                /* Save the original string pointer */

.find_end:
    /* Move to the end of the string */
    cmpb   $0, (%rdi)          /* Check for NUL terminator */
    je     .trim               /* If found, start trimming */
    inc    %rdi                /* Move to next character */
    jmp    .find_end           /* Loop until NUL terminator is found */

.trim:
    dec    %rdi                /* Move one character back from the NUL terminator */

.check_alpha:
    mov    (%rdi), %al         /* Load the current character into al */

    call   wc_isalpha          /* Call the wc_isalpha function */
    
    test   %al, %al            /* Test the result of wc_isalpha */
    jnz    .end                /* If non-zero, it's an alphabetic character, so we're done */

    /* If not alphabetic, set to NUL terminator */
    movb   $0, (%rdi)          /* Replace with NUL terminator */
    dec    %rdi                /* Move to previous character */
    jmp    .check_alpha        /* Check the previous character */

.end:
    pop    %rdi                /* Restore the original string pointer */
    leave                      /* Cleanup stack and restore base pointer */
    ret                        /* Return */


/*




 * Sebastian Rincon
 * Search the specified linked list of WordEntry objects for an object
 * containing the specified string.
 *
 * If a matching object is found, set the int variable pointed-to by
 * inserted to 0 and return a pointer to the matching object.
 *
 * If a matching object is not found, allocate a new WordEntry object,
 * set its next pointer to point to what head points to (i.e., so the
 * new object is at the head of the list), set the variable pointed-to
 * by inserted to 1, and return a pointer to the new node. Note that
 * the new node should have its count value set to 0. (It is the caller's
 * job to update the count.)
 *
 * C function prototype:
 *    struct WordEntry *wc_find_or_insert(struct WordEntry *head, const unsigned char *s, int *inserted);
 */
	.globl wc_find_or_insert
wc_find_or_insert:
	/* TODO: implement */
	ret

/*
 * Sebastian Rincon
 * Find or insert the WordEntry object for the given string (s), returning
 * a pointer to it. The head of the linked list which contains (or should
 * contain) the entry for s is the element of buckets whose index is the
 * hash code of s mod num_buckets.
 *
 * Returns a pointer to the WordEntry object in the appropriate linked list
 * which represents s.
 *
 * C function prototype:
 *    struct WordEntry *wc_dict_find_or_insert(struct WordEntry *buckets[], unsigned num_buckets, const unsigned char *s);
 */
	.globl wc_dict_find_or_insert
wc_dict_find_or_insert:
	/* TODO: implement */
	ret

/*
 * Sebastian Rincon
 * Free all of the nodes in given linked list of WordEntry objects.
 *
 * C function prototype:
 *    void wc_free_chain(struct WordEntry *p);
 */
	.globl wc_free_chain
wc_free_chain:
	/* TODO: implement */
	ret

/*
vim:ft=gas:
*/
