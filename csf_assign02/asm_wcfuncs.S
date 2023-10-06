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
	push   %rbp  /* Save old base pointer */
	mov    %rbp, %rsp  /* Set new base pointer */

.Lcompreloop:
	mov    (%rdi), %al /* Load byte from address in rdi to al */

	mov    (%rsi), %dl /* Load byte from address in rsi to dl */

	cmp    %dl, %al   /* are two chars equal? */
	jne    .doneCompare  /* If no, exit the loop */

	test   %al, %al            /* is al null?*/
	je     .doneCompare  /* If yes, exit the loop */

	/* Move to the next characters */
	inc    %rdi     /* Move to next byte of lhs */
	inc    %rsi     /* Move to next byte of rhs */
	jmp    .Lcompreloop   /* continue loop */

.doneCompare:
	sub    %dl, %al  /* Subtract dl (rhs byte) from al (lhs byte) */
	movzx  %al, %eax    /* Zero-extend al to eax for the return value */

	/* end and clean */
	leave    /* cleanup */
	ret   /* Return with difference in eax */


/*
 * Copy NUL-terminated source string to the destination buffer.
 *
 * C function prototype:
 *    void wc_str_copy(unsigned char *dest, const unsigned char *source);
 */
    .globl wc_str_copy
wc_str_copy:
	pushq  %rbp    /* Save old base pointer */
	movq   %rsp, %rbp   /* Set new base pointer */

.copy_loop:
	movb   (%rsi), %al  /* Load byte from address in rsi (source) to al */

	/* Store the byte in destination */
	movb   %al, (%rdi)  /* Store byte from al to address in rdi (destination) */

	testb  %al, %al   /* is al null? */
	je     .end_copy   /* If yes, exit the loop */

	/* Move to the next characters */
	incq   %rdi     /* Move to next byte of destination */
	incq   %rsi   /* Move to next byte of source */
	jmp    .copy_loop          /* continue loop */

.end_copy:
	leave /* Cleanup */
	ret  

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
    cmpb $0x20, %dil         	/* is char space? */
    je .is_space           		/* If yes, it's a whitespace */

    /* Compare c with '\t' */
    cmpb $0x09, %dil         	/* ic char tab? */
    je .is_space           		/* If yes, it's a whitespace */

    /* Compare c with '\r' */
    cmpb $0x0d, %dil         	/* is char return? */
    je .is_space           		/* If yes, it's a whitespace */

    /* Compare c with '\n' */
    cmpb $0x0a, %dil         	/* is char newline? */
    je .is_space           		/* If yes, it's a whitespace */

    /* Compare c with '\f' */
    cmpb $0x0c, %dil         	/* is char feed? */
    je .is_space           		/* If yes, it's a whitespace */

    /* Compare c with '\v' */
    cmpb $0x0b, %dil         	/* is char vertical tab? */
    je .is_space           		/* If yes, it's a whitespace */

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
    pushq %rbp
    movq %rsp, %rbp
    pushq %rbx
    pushq %r12
    pushq %r13
    subq $32, %rsp /* Align stack */

    movq %rsi, %r13 /* Initialize r13 with the address of the buffer */
    xorq %r12, %r12 /* r12 will be our index 'i' */
    movq %rdi, %rbx /* rbx will hold our FILE* 'in' */

.LskipWhitespace:
    movq %rbx, %rdi /* Restore FILE* in to rdi for fgetc */
    call fgetc
    cmp $-1, %eax /* Check for EOF */
    je .end_no_word
    mov %al, %sil
    mov %rsi, %rdi /* Move the character to rdi for wc_isspace call */
    call wc_isspace
    test %eax, %eax /* If it's a whitespace, loop again */
    jnz .LskipWhitespace

.LreadWord:
    cmpq $MAX_WORDLEN, %r12    /* Check if buffer is full */
    je .terminate_word
    movb %sil, (%r13) /* Store character into buffer */
    incq %r12 /* Increment index */
    incq %r13 /* Move to next buffer position */
    movq %rbx, %rdi /* Restore FILE* in to rdi for fgetc */
    call fgetc /* Read next character */
    cmp $-1, %eax /* Check for EOF */
    je .terminate_word
    mov %al, %sil
    mov %rsi, %rdi /* Move the character to rdi for wc_isspace call */
    call wc_isspace
    test %eax, %eax /* If it's not whitespace, loop */
    jz .LreadWord

.terminate_word:
    movb $0, (%r13) /* Null-terminate the buffer */
    movl $1, %eax /* Return 1 */
    addq $32, %rsp
    popq %r13
    popq %r12
    popq %rbx
    popq %rbp
    ret

.end_no_word:
    movl $0, %eax  /* Return 0 */
    addq $32, %rsp
    popq %r13
    popq %r12
    popq %rbx
    popq %rbp
    ret

/*
 * Convert the NUL-terminated character string in the array
 * pointed-to by w so that every letter is lower-case.
 *
 * C function prototype:
 *    void wc_tolower(unsigned char *w);
 */
	.globl wc_tolower
wc_tolower:
	subq $8, %rsp			 /* Align stack */

.Lstr_tolower_loop:
	movb (%rdi), %al		/* Get character */
	cmpb $0, %al			/* Ais character /0? */
	je .Lstr_tolower_done	/* If yes, strting over */

	cmpb $65, %al			/* Is character value below 65? */
	jb .Lstr_tolower_loop_continue /* If yes, not a uppercase character */
	cmpb $90, %al			/* Is character value above 90? */
	ja .Lstr_tolower_loop_continue /* If yes, not a uppercase character */

	addb $(97 - 65), %al		/* Add difference to character */
	movb %al, (%rdi)			/* Move modified character back to string */

.Lstr_tolower_loop_continue:
	incq %rdi					/* Move ahead in string */
	jmp .Lstr_tolower_loop		/* Continue loop */

.Lstr_tolower_done:
	addq $8, %rsp			/* Realign stack */
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
    // Prologue
    pushq %rbp /* Save old base pointer */
    movq  %rsp, %rbp /* Set new base pointer */
    subq  $16, %rsp  /* Reserve space for local variable 'length' */
    movq  %rdi, -16(%rbp) /* Save the pointer w */
    movl  $0, -8(%rbp) /* Initialize length to 0 */

.Lcalculate_length:
    movl  -8(%rbp), %eax /* Load length into eax */
    movq  -16(%rbp), %rdx /* Load w into rdx */
    movzbl (%rdx,%rax), %ecx /* Load w[length] into ecx */
    testb %cl, %cl /* is cl (NUL)? */
    je    .Ltrim /* If yes, trim */
    incl  -8(%rbp) /* Increment length */
    jmp   .Lcalculate_length /* continue loop */

.Ltrim:
.Ltrim_loop:
    cmpl  $0, -8(%rbp) /* is length <= 0? */
    jle   .Lset_null /* If yes, set null terminator */
    movl  -8(%rbp), %eax /* Load length into eax */
    decq  %rax /* Decrement to get w[length - 1] */
    movq  -16(%rbp), %rdx   /* Load pointer w into rdx */
    movzbl (%rdx,%rax), %ecx /* Load w[length - 1] into ecx */
    movb  %cl, %dil     /* Move the byte to dil (byte register) */
    call  wc_isalpha /* Call wc_isalpha */
    testb %al, %al  /* is character null? */
    jnz   .Lset_null /* If no, set null terminator */
    decl  -8(%rbp)   /* Decrement length */
    jmp   .Ltrim_loop   /* continue loop */

.Lset_null:
    movl  -8(%rbp), %eax   /* Load length into eax */
    movq  -16(%rbp), %rdx  /* Load w into rdx */
    movb  $0, (%rdx,%rax)  /* Set w[length] to '\0' (null terminator) */

    leave     /* cleanup */
    ret

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
    movq %rdi, %r10            # Copy head to current (current = head)
    movq %rsi, %r11            # Copy s to rcx (s = rcx)
    movq %rdx, %r15            # Copy inserted to rdi (inserted = rdi)
    pushq %rbp
    movq %rbp, %rsp
    pushq %r15
    pushq %r11
    pushq %r10

.Lsearch_loop:
    pop %r10
    movq %r10, %r8
    pushq %r10
    cmpq $0, %r8              # Check if current is NULL
    je .Linsert_new_node

    pop %r10
    pop %r11
    pop %r15
    movq %r15, %rdx        # Load &current->word into rdx (word pointer)
    # pushq %rax                 # Save current on the stack
    movq %r11, %rdi
    movq %r15, %rsi
    pushq %r15
    pushq %r11
    pushq %r10
    call wc_str_compare        # Call wc_str_compare(lhs, rhs)

     addq $8, %rsp              # Pop the current from the stack

    testq %rax, %rax           # Check if wc_str_compare result is 0
    jz .Lfound_match

    pop %r10
    movq 72(%r10), %r10        # Move to the next node (current = current->next)
    pushq %r10
    jmp .Lsearch_loop

.Lfound_match:
    movl $0, (%rdi)            # Set inserted to 0 (matching object found)
    movq %rax, %rax            # Return the pointer to the matching node
    ret

.Linsert_new_node:
    popq %r10
    popq %r11
    popq %r15
    # Allocate memory for the new WordEntry object
    subq $80, %rsp
    movq %rsp, %rax
    
    /*movq $80, %rdi             # Size of WordEntry struct (80 bytes)
    call malloc                # Call malloc to allocate memory
    testq %rax, %rax           # Check if malloc failed
    jz malloc_failed*/

    movq %r11, 0(%rax)          # Copy the string 's' into word
    movl $0, 64(%rax)          # Initialize count to 0
    movq %r10, 72(%rax)        # Set next to point to the current head
    movq $1, %r15            # Set inserted to 1 (new object inserted)
    addq $80, %rsp
    popq %rbp
    ret

malloc_failed:
    movl $-1, (%rdi)           # Set inserted to -1 (malloc failure)
    xorq %rax, %rax            # Return NULL
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
    movq %rdi, %r12 # loads hashtable to r12
    movq %rsi, %r13 # loads number of buckets to r13
    movq %rdx, %r14 # loads string to look/insert to r14

    pushq %r14 # pushes string into stack
    pushq %r13 # pushes num buckets into stack
    pushq %r12 # pushes hashtable to stack

    movq %r14, %rdi # sets parameter to the string
    call wc_hash # calls hashfunction to get hash number from string
    popq %r12
    popq %r13
    movq %r13, %rcx # move numbukets to to r9 to be divosor
    xorq %rdx, %rdx
    divq %rcx # divide rax by rx, result is in rax, remainder in rdx
    movq %rdx, %r8 # move remainder value to r8
    pop %r14 # pops string
    movq %r14, %rsi # sets string as 2nd parameter
    leaq 0(%r12, %r8, 8), %r10 # finds address of start of linked list
    movq (%r10), %rdi # sets headofLink list as parameter 1
    movq $0, %r9
    movq %r9, %rdx

    pushq %r14
    pushq %r13
    pushq %r12
    pushq %r8
    pushq %rdx

    call wc_find_or_insert

    popq %rdx
    push %rax
    movq %rdx, %r10
    cmpq $1, %r10
    je .Lupdate_head
    pop %rax
    popq %r8
    popq %r12
    popq %r13
    popq %r14
    ret

.Lupdate_head:
    pop %rax
    popq %r8
    popq %r12
    popq %r13
    popq %r14
    leaq (%r12, %r8, 8), %r9
    movq %r12 , %r9
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
    pushq %rbp
    movq %rsp, %rbp
    pushq %r12
    pushq %r13                # %r13 will be used to temporarily hold the next node

    movq %rdi, %r12           # Move head of the list to %r12

.L_free_start:
    testq %r12, %r12          # Test if current node (in %r12) is NULL
    je .L_free_end                  # If NULL, jump to end

    movq 72(%r12), %r13      # Load next pointer to %r13 FIRST before freeing current node
    movq %r12, %rdi          # Set current node to be freed
    call free                 # Call free on the current node

    movq %r13, %r12          # Move to the next node using %r13

    jmp .L_free_start          # Jump back to the loop start

.L_free_end:
    popq %r13
    popq %r12                 # Restore registers
    leave
    ret



/*
vim:ft=gas:
*/