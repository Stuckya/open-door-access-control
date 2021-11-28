package com.github.stuckya.dooraccessserver.domain.persistence;

import lombok.Builder;
import lombok.Getter;
import lombok.NoArgsConstructor;
import lombok.experimental.SuperBuilder;

import javax.persistence.Entity;
import javax.persistence.JoinColumn;
import javax.persistence.OneToMany;
import javax.validation.constraints.NotBlank;
import javax.validation.constraints.Size;
import java.util.HashSet;
import java.util.Set;

@SuperBuilder
@Getter
@NoArgsConstructor
@Entity
public class User extends BaseEntity<User> {

    @NotBlank
    private String username;

    @NotBlank
    @Size(max = 320, message = "Email must be valid length.")
    private String email;

    @NotBlank
    @Size(min = 10, message = "Password must be at least 10 characters in length.")
    private String password;

    @Builder.Default
    @OneToMany
    @JoinColumn(name = "id")
    // TODO: Join Table?
    private final Set<Credential<?>> credentials = new HashSet<>();
}
