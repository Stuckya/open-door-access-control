package com.github.stuckya.dooraccessserver.repositories;


import com.github.stuckya.dooraccessserver.domain.persistence.Credential;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import java.util.UUID;

@Repository
public interface CredentialRepository extends JpaRepository<Credential<?>, UUID> {
}
