#lang racket
(require "scanning.rkt")

(define pc 0)
(define counter -4)
(define labels '())
(define instructions '())

;;ERROR
(define (create-error s)
  (begin
    (displayln s (current-error-port))
    (exit)))

;; Scannig for labels
(define (labelize tokens)
  (cond [(empty? tokens) '()]
        [(label? tokens) (begin (update-labels tokens) (labelize (rest tokens)))]
        [else (begin (set! pc (+ pc 4)) (set! instructions (cons tokens instructions)))]))


; Main assembler
(define (assemble tokens)
  (set! counter (+ 4 counter))
  (cond [(empty? tokens) (set! pc (- counter 4))]

        [(word-instr? tokens) (word (second tokens))]

        [(one-reg-check tokens "jr") (jump (second tokens) 8)]
        [(one-reg-check tokens "jalr") (jump (second tokens) 9)]

        [(one-reg-check tokens "mfhi") (move-and-load (second tokens) 16)]
        [(one-reg-check tokens "mflo") (move-and-load (second tokens) 18)]
        [(one-reg-check tokens "lis") (move-and-load (second tokens) 20)]

        [(bool-check tokens "beq") (bool (rest tokens) 4)]
        [(bool-check tokens "bne") (bool (rest tokens) 5)]

        [(three-reg-check tokens "add") (three-reg-convert (rest tokens) 32)]
        [(three-reg-check tokens "sub") (three-reg-convert (rest tokens) 34)]
        [(three-reg-check tokens "slt") (three-reg-convert (rest tokens) 42)]
        [(three-reg-check tokens "sltu") (three-reg-convert (rest tokens) 43)]

        [else (create-error "ERROR: INVALID INSTRUCTION")]))


;;WORD FUNCTIONS
;Word Argument
(define (word token)
  (cond [(equal? (token-kind token) 'INT) (wordint (string->number (token-lexeme token)))]
        [(equal? (token-kind token) 'HEXINT) (wordhex (token-lexeme token))]
        [(equal? (token-kind token) 'ID) (wordint (lookup labels (token-lexeme token)))]
        [else (create-error "ERROR: INVALID WORD ARGUMENT")]))


;Print int in bytes
(define (wordint i)
  (begin (define j (word-range i 32))
         (output-bytes j)))

;checks the range of ints
(define (word-range i n)
  (cond [(and (>= i 0) (<= i (- (expt 2 n) 1))) i]
        [(and (>= i (* -1 (expt 2 n))) (< i 0))  (+ (expt 2 n) i)]
        [else (create-error "ERROR: INTEGER OUT OF BOUNDS")]))


;prints hex in bytes
(define (wordhex s)
  (define s1 (substring s 2))
  (wordint (hex->int s1)))


;Converts hex to decimal
(define (hex->int s)
  (define (helper s acc)
    (cond [(= (string-length s) 1) (+ (* 16 acc) (hextable s))]
          [else (helper (substring s 1) (+ (* 16 acc) (hextable (substring s 0 1))))]))
  (helper s 0))

;convert hex digit to int
(define (hextable s)
  (define f (string-downcase s))
  (cond [(string=? "a" f) 10]
        [(string=? "b" f) 11]
        [(string=? "c" f) 12]
        [(string=? "d" f) 13]
        [(string=? "e" f) 14]
        [(string=? "f" f) 15]
        [else (string->number s)]))


;;LABELS
;;Correct the last possible labels
(define (fix-labels label-list)
  (cond [(empty? label-list) '()]
        [(> (second (first label-list)) pc)
         (cons (list (first (first label-list)) pc)
               (fix-labels (rest label-list)))]
        [else (cons (first label-list)
                    (fix-labels (rest label-list)))]))

;;checks for duplicate labels
(define (duplicate-label? curr)
  (foldl (lambda (a res) (or res (string=? (first a) curr))) #f labels))


;;Lookup
(define (lookup array label)
  (cond [(empty? array) (create-error "ERROR: UNDEFINED LABEL")]
        [(string=? label (first (first array))) (second (first array))]
        [else (lookup (rest array) label)]))

(define (update-labels tokens)
  (define curr-label (substring (token-lexeme (first tokens)) 0 (- (string-length (token-lexeme (first tokens))) 1)))
  (if (duplicate-label? curr-label)
      (create-error "ERROR: DUPLICATE LABELS")
      (set! labels (cons (list curr-label pc) labels))))

;;BOOLEANS
;;check register
(define (check-register i)
  (if (or (< i 0) (> i 31)) (create-error "ERROR: INVALID REGISTER") #t))

;;Checks for labels
(define (label? tokens)
  (equal? (token-kind (first tokens)) 'LABEL))

(define (word-instr? tokens)
  (and (= 2 (length tokens)) (equal? (token-kind (first tokens)) 'WORD)))

(define (one-reg-check tokens s)
  (displayln (instruction? tokens))
  (displayln tokens)
  (and (instruction? tokens)
       (string=? (token-lexeme (first tokens)) s)
       (check-register (string->number (substring (token-lexeme (second tokens)) 1)))))

(define (three-reg-check tokens s)
  (and (instruction? tokens)
       (string=? (token-lexeme (first tokens)) s)
       (equal? (token-kind (sixth tokens)) 'REG)
       (check-register (string->number (substring (token-lexeme (second tokens)) 1)))
       (check-register (string->number (substring (token-lexeme (fourth tokens)) 1)))
       (check-register (string->number (substring (token-lexeme (sixth tokens)) 1)))))

(define (bool-check tokens s)
  (and (instruction? tokens)
       (string=? (token-lexeme (first tokens)) s)
       (or (equal? (token-kind (sixth tokens)) 'INT)
           (equal? (token-kind (sixth tokens)) 'HEXINT)
           (equal? (token-kind (sixth tokens)) 'ID))
       (check-register (string->number (substring (token-lexeme (second tokens)) 1)))
       (check-register (string->number (substring (token-lexeme (fourth tokens)) 1)))))

;; Check si instruction is valid
(define (instruction? tokens)
  (or (and (= 2 (length tokens))
           (valid-single-id? (token-lexeme (first tokens)))
           (or (equal? (token-kind (first tokens)) 'WORD)
               (and (equal? (token-kind (first tokens)) 'ID)
                    (equal? (token-kind (second tokens)) 'REG))))
      
      (and (= 6 (length tokens))
           (valid-triple-id? (token-lexeme (first tokens)))
           (and (equal? (token-kind (first tokens)) 'ID)
                (equal? (token-kind (second tokens)) 'REG)
                (equal? (token-kind (third tokens)) 'COMMA)
                (equal? (token-kind (fourth tokens)) 'REG)
                (equal? (token-kind (fifth tokens)) 'COMMA)
                (or
                  (equal? (token-kind (sixth tokens)) 'REG)
                  (equal? (token-kind (sixth tokens)) 'INT)
                  (equal? (token-kind (sixth tokens)) 'HEXINT)
                  (equal? (token-kind (sixth tokens)) 'ID))))
      
      (and (= 2 (length tokens))
           (equal? (token-kind (first tokens)) 'ID)
           (equal? (token-kind (second tokens)) 'REG)
           (valid-single-id? (token-lexeme (first tokens))))))

           ;;Checks if an id is valid
(define (valid-single-id? id)
  (define ids '("jr" "jalr" "mfhi" "mflo" "lis"))
  (list? (member id ids)))


(define (valid-triple-id? id)
  (define ids'("add" "sub" "slt" "sltu" "beq" "bne"))
  (list? (member id ids)))

;;JUMP
(define (jump token op)
  (define i (string->number (substring (token-lexeme token) 1)))
  (define reg (arithmetic-shift i 21))
  (define res (bitwise-ior reg op))
  (output-bytes res))


(define (three-reg-convert tokens op)
  (define i (string->number (substring (token-lexeme (first tokens)) 1)))
  (define j (string->number (substring (token-lexeme (third tokens)) 1)))
  (define k (string->number (substring (token-lexeme (fifth tokens)) 1)))
  (define r1 (arithmetic-shift j 21))
  (define r2 (arithmetic-shift k 16))
  (define r3 (arithmetic-shift i 11))
  (define res (bitwise-ior r1 r2 r3 op))
  (output-bytes res))

(define (move-and-load tokens op)
  (define i (string->number (substring (token-lexeme tokens) 1)))
  (define reg (arithmetic-shift i 11))
  (define res (bitwise-ior reg op))
  (output-bytes res))


(define (bool tokens op)
  (define j (string->number (substring (token-lexeme (first tokens)) 1)))
  (define k (string->number (substring (token-lexeme (third tokens)) 1)))
  (define i
    (cond [(equal? (token-kind (fifth tokens)) 'INT)
           (word-range (string->number (token-lexeme (fifth tokens))) 16)]

          [(equal? (token-kind (fifth tokens)) 'HEXINT)
           (word-range (hex->int (substring (token-lexeme (fifth tokens)) 2)) 16)]

          [else (word-range (- (quotient (- (lookup labels (token-lexeme (fifth tokens))) counter) 4) 1) 16)]))
  (define r1 (arithmetic-shift op 26))
  (define r2 (arithmetic-shift j 21))
  (define r3 (arithmetic-shift k 16))
  (define res (bitwise-ior r1 r2 r3 i))
  (output-bytes res))

(define (output-bytes j)
  (write-byte (arithmetic-shift j -24))
  (write-byte (bitwise-and (arithmetic-shift j -16) #x00FF))
  (write-byte (bitwise-and (arithmetic-shift j -8) #x0000FF))
  (write-byte (bitwise-and j #x000000FF)))

;; For now, just scan the entirety of standard input and print it back to
;; standard output
(for [(line (in-lines))] (displayln (scan line)))

(begin
  (for [(line (in-lines))] (labelize (scan line)))
  (set! labels (fix-labels labels))
  ;(for [(i labels)] (fprintf (current-error-port) "~a ~a~n" (first i) (second i)))
  (for [(instr (reverse instructions))] (assemble instr)))