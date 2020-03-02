$ set verify
$ CFLAGS :== /include=[]/warnings=(disable=UNSTRUCTMEM)/standard=relaxed
$ cc 'CFLAGS a_cmd.c
$ cc 'CFLAGS back_cmd.c
$ cc 'CFLAGS brace_cmd.c
$ cc 'CFLAGS bracket_cmd.c
$ cc 'CFLAGS build_str.c
$ cc 'CFLAGS case_cmd.c
$ cc 'CFLAGS cmd_buf.c
$ cc 'CFLAGS cmd_exec.c
$ cc 'CFLAGS cmd_scan.c
$ cc 'CFLAGS cmd_tables.c
$ cc 'CFLAGS comma_cmd.c
$ cc 'CFLAGS ctrl_a_cmd.c
$ cc 'CFLAGS ctrl_c_cmd.c
$ cc 'CFLAGS ctrl_t_cmd.c
$ cc 'CFLAGS ctrl_u_cmd.c
$ cc 'CFLAGS ctrl_v_cmd.c
$ cc 'CFLAGS ctrl_w_cmd.c
$ cc 'CFLAGS datetime_cmd.c
$ cc 'CFLAGS delete_cmd.c
$ cc 'CFLAGS digit_cmd.c
$ cc 'CFLAGS ea_cmd.c
$ cc 'CFLAGS eb_cmd.c
$ cc 'CFLAGS ec_cmd.c
$ cc 'CFLAGS ef_cmd.c
$ cc 'CFLAGS eg_cmd.c
$ cc 'CFLAGS ei_cmd.c
$ cc 'CFLAGS ek_cmd.c
$ cc 'CFLAGS el_cmd.c
$ cc 'CFLAGS em_cmd.c
$ cc 'CFLAGS en_cmd.c
$ cc 'CFLAGS env_sys.c
$ cc 'CFLAGS ep_cmd.c
$ cc 'CFLAGS e_pct_cmd.c
$ cc 'CFLAGS eq_cmd.c
$ cc 'CFLAGS equals_cmd.c
$ cc 'CFLAGS er_cmd.c
$ cc 'CFLAGS errors.c
$ cc 'CFLAGS estack.c
$ cc 'CFLAGS e_ubar_cmd.c
$ cc 'CFLAGS ew_cmd.c
$ cc 'CFLAGS ex_cmd.c
$ cc 'CFLAGS fb_cmd.c
$ cc 'CFLAGS fd_cmd.c
$ cc 'CFLAGS file.c
$ cc 'CFLAGS file_sys.c
$ cc 'CFLAGS fk_cmd.c
$ cc 'CFLAGS flag_cmd.c
$ cc 'CFLAGS fr_cmd.c
$ cc 'CFLAGS gapbuf.c
$ cc 'CFLAGS getopt.c
$ cc 'CFLAGS g_cmd.c
$ cc 'CFLAGS goto_cmd.c
$ cc 'CFLAGS if_cmd.c
$ cc 'CFLAGS insert_cmd.c
$ cc 'CFLAGS loop_cmd.c
$ cc 'CFLAGS m_cmd.c
$ cc 'CFLAGS memory.c
$ cc 'CFLAGS move_cmd.c
$ cc 'CFLAGS n_cmd.c
$ cc 'CFLAGS option_sys.c
$ cc 'CFLAGS p_cmd.c
$ cc 'CFLAGS pct_cmd.c
$ cc 'CFLAGS print_cmd.c
$ cc 'CFLAGS q_cmd.c
$ cc 'CFLAGS qreg.c
$ cc 'CFLAGS quest_cmd.c
$ cc 'CFLAGS radix_cmd.c
$ cc 'CFLAGS s_cmd.c
$ cc 'CFLAGS search.c
$ cc 'CFLAGS teco.c
$ cc 'CFLAGS term_buf.c
$ cc 'CFLAGS term_in.c
$ cc 'CFLAGS term_out.c
$ cc 'CFLAGS term_sys.c
$ cc 'CFLAGS type_cmd.c
$ cc 'CFLAGS ubar_cmd.c
$ cc 'CFLAGS u_cmd.c
$ cc 'CFLAGS value_cmd.c
$ cc 'CFLAGS w_cmd.c
$ cc 'CFLAGS window.c
$ cc 'CFLAGS x_cmd.c
$ cc 'CFLAGS yank_cmd.c
$ link a_cmd, back_cmd, brace_cmd, bracket_cmd, build_str, case_cmd, -
       cmd_buf, cmd_exec, cmd_scan, cmd_tables, comma_cmd, ctrl_a_cmd, -
       ctrl_c_cmd, ctrl_t_cmd, ctrl_u_cmd, ctrl_v_cmd, ctrl_w_cmd, -
       datetime_cmd, delete_cmd, digit_cmd, ea_cmd, eb_cmd, ec_cmd, -
       ef_cmd, eg_cmd, ei_cmd, ek_cmd, el_cmd, em_cmd, en_cmd, -
       env_sys, ep_cmd, e_pct_cmd, eq_cmd, equals_cmd, er_cmd, -
       errors, estack, e_ubar_cmd, ew_cmd, ex_cmd, fb_cmd, fd_cmd, -
       file, file_sys, fk_cmd, flag_cmd, fr_cmd, gapbuf, getopt, -
       g_cmd, goto_cmd, if_cmd, insert_cmd, loop_cmd, m_cmd, -
       memory, move_cmd, n_cmd, option_sys, p_cmd, pct_cmd, print_cmd,
       q_cmd, qreg, quest_cmd, radix_cmd, s_cmd, search, teco, -
       term_buf, term_in, term_out, term_sys, type_cmd, ubar_cmd, -
       u_cmd, value_cmd, w_cmd, window, x_cmd, yank_cmd
